import { MongoClient } from 'mongodb';
import { HttpMethod } from './core/api';
import { Log } from './core/logging';
import { ExpressRouter } from './core/routing/express/express-router';
import { AuthenticationHelper } from './domain/auth/authentication-helper';
import { CreateDataPointHandler, LoginHandler, RegistrationHandler } from './endpoints';
import { CreateDeviceCodeHandler } from './endpoints/create-device-code/create-device-code.handler';
import { CreateDeviceHandler } from './endpoints/create-device/create-device.handler';
import { CreateTriggerHandler } from './endpoints/create-trigger/create-trigger.handler';
import { DeleteDeviceHandler } from './endpoints/delete-device/delete-device.handler';
import { DeleteTriggerHandler } from './endpoints/delete-trigger/delete-trigger.handler';
import { GetDeviceDataHandler } from './endpoints/get-device-data/get-device-data.handler';
import { GetDeviceHandler } from './endpoints/get-device/get-device.handler';
import { GetDevicesHandler } from './endpoints/get-devices/get-devices.handler';
import { GetSensorDataHandler } from './endpoints/get-sensor-data/get-sensor-data.handler';
import { GetTriggersHandler } from './endpoints/get-triggers/get-triggers.handler';
import { LoginDeviceHandler } from './endpoints/login-device/login-device.handler';
import { DataPointInfo } from './models/data-point.info';
import { DeviceCodeInfo } from './models/device-code.info';
import { DeviceInfo } from './models/device.info';
import { TriggerInfo } from './models/trigger.info';
import { UserInfo } from './models/user.info';

async function main() {
  Log.info('running server ...');

  const port = process.env.AIR_SENSE_PORT || 7325;
  const mongoUrl = process.env.AIR_SENSE_MONGO_URL || 'mongodb://localhost:27017';

  const secret = process.env.AIR_SENSE_SECRET;

  if (!secret) {
    Log.error('could not find token secret ...');
    Log.error('shutting down ...');

    return;
  }

  Log.info('connecting to database ...');
  const client = new MongoClient(mongoUrl);

  try {
    await client.connect();
  } catch (error) {
    Log.error('failed to connect to database:', error);
    Log.error('shutting down ...');

    return;
  }

  const db = client.db('airsense');

  const userCollection = db.collection<UserInfo>('users');
  const deviceCollection = db.collection<DeviceInfo>('devices');
  const deviceCodeCollection = db.collection<DeviceCodeInfo>('deviceCodes');
  const dataCollection = db.collection<DataPointInfo>('data');
  const triggerCollection = db.collection<TriggerInfo>('triggers');

  deviceCodeCollection.createIndex({ createdOn: 1 }, { expireAfterSeconds: 60 * 60 * 24 });

  Log.info('launching router engine ...');

  const router = new ExpressRouter();
  const authenticationHelper = new AuthenticationHelper(secret);

  router.route(HttpMethod.POST, '/api/v1/users/register', new RegistrationHandler(userCollection));
  router.route(HttpMethod.POST, '/api/v1/users/login', new LoginHandler(userCollection, authenticationHelper));

  router.route(HttpMethod.GET, '/api/v1/devices', new GetDevicesHandler(deviceCollection, authenticationHelper));
  router.route(HttpMethod.GET, '/api/v1/devices/:id', new GetDeviceHandler(deviceCollection, authenticationHelper));
  router.route(
    HttpMethod.GET,
    '/api/v1/devices/:id/data',
    new GetDeviceDataHandler(dataCollection, authenticationHelper),
  );
  router.route(HttpMethod.POST, '/api/v1/devices', new CreateDeviceHandler(deviceCollection, authenticationHelper));
  router.route(
    HttpMethod.DELETE,
    '/api/v1/devices/:id',
    new DeleteDeviceHandler(deviceCollection, dataCollection, authenticationHelper),
  );
  router.route(
    HttpMethod.POST,
    '/api/v1/devices/:id/code',
    new CreateDeviceCodeHandler(deviceCollection, deviceCodeCollection, authenticationHelper),
  );
  router.route(
    HttpMethod.POST,
    '/api/v1/devices/login',
    new LoginDeviceHandler(deviceCodeCollection, authenticationHelper),
  );

  router.route(
    HttpMethod.GET,
    '/api/v1/devices/:id/triggers',
    new GetTriggersHandler(triggerCollection, authenticationHelper),
  );
  router.route(
    HttpMethod.POST,
    '/api/v1/devices/:id/trigger',
    new CreateTriggerHandler(triggerCollection, authenticationHelper),
  );
  router.route(
    HttpMethod.DELETE,
    '/api/v1/triggers/:id',
    new DeleteTriggerHandler(triggerCollection, authenticationHelper),
  );

  router.route(
    HttpMethod.POST,
    '/api/v1/data',
    new CreateDataPointHandler(dataCollection, triggerCollection, authenticationHelper),
  );

  router.route(
    HttpMethod.GET,
    '/api/v1/sensors/latest',
    new GetSensorDataHandler(dataCollection, authenticationHelper),
  );

  router.run(port, () => {
    Log.info(`server up and running on port: ${port}`);
  });
}

main().catch(console.error);
