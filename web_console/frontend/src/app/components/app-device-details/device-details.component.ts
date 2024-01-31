import { CommonModule } from '@angular/common';
import { AfterContentInit, Component, ElementRef, TemplateRef, ViewChild } from '@angular/core';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { MatButtonModule } from '@angular/material/button';
import { MatCardModule } from '@angular/material/card';
import { MatCheckboxModule } from '@angular/material/checkbox';
import {
  MatDialog,
  MatDialogActions,
  MatDialogClose,
  MatDialogContent,
  MatDialogTitle,
} from '@angular/material/dialog';
import { MatExpansionModule } from '@angular/material/expansion';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { MatSelectChange, MatSelectModule } from '@angular/material/select';
import { MatSnackBar } from '@angular/material/snack-bar';
import { MatToolbarModule } from '@angular/material/toolbar';
import { ActivatedRoute, Router, RouterModule, RouterOutlet } from '@angular/router';
import { Chart } from 'chart.js/auto';
import 'chartjs-adapter-date-fns';
import { enUS } from 'date-fns/locale';
import { DeviceInfo, DeviceService } from '../../services/backend/device.service';
import { TriggerInfo, TriggerService } from '../../services/backend/trigger.service';
import { LogService } from '../../services/logging/log.service';

/**
 * The device details component.
 */
@Component({
  selector: 'app-device-details',
  standalone: true,
  imports: [
    CommonModule,
    RouterModule,
    RouterOutlet,
    MatButtonModule,
    MatToolbarModule,
    MatIconModule,
    MatCardModule,
    FormsModule,
    MatInputModule,
    MatFormFieldModule,
    MatCheckboxModule,
    MatExpansionModule,
    MatDialogActions,
    MatDialogClose,
    MatDialogTitle,
    MatDialogContent,
    MatFormFieldModule,
    MatSelectModule,
    FormsModule,
    ReactiveFormsModule,
  ],
  templateUrl: './device-details.component.html',
  styleUrls: ['./device-details.component.scss'],
})
export class DeviceDetailsComponent implements AfterContentInit {
  /**
   * A reference to the canvas element for the humidity chart.
   */
  @ViewChild('canvasHumidity', { static: true })
  public canvasHumidity: ElementRef<HTMLCanvasElement> | undefined;

  /**
   * A reference to the canvas element for the pressure chart.
   */
  @ViewChild('canvasPressure', { static: true })
  public canvasPressure: ElementRef<HTMLCanvasElement> | undefined;

  /**
   * A reference to the canvas element for the temperature chart.
   */
  @ViewChild('canvasTemperature', { static: true })
  public canvasTemperature: ElementRef<HTMLCanvasElement> | undefined;

  /**
   * A reference to the canvas element for the gas resistance chart.
   */
  @ViewChild('canvasGasResistance', { static: true })
  public canvasGasResistance: ElementRef<HTMLCanvasElement> | undefined;

  /**
   * A reference to the dialog template for authorizing a device.
   */
  @ViewChild('dialogAuthorize', { static: true })
  public dialogAuthorize: TemplateRef<unknown> | undefined;

  /**
   * A reference to the dialog template for deleting a device.
   */
  @ViewChild('dialogDelete', { static: true })
  public dialogDelete: TemplateRef<unknown> | undefined;

  /**
   * A reference to the dialog template for creating a trigger.
   */
  @ViewChild('dialogTrigger', { static: true })
  public dialogTrigger: TemplateRef<unknown> | undefined;

  /**
   * The id of the device.
   */
  protected deviceId: string | null;

  /**
   * The device info.
   */
  protected deviceInfo: DeviceInfo | null = null;

  /**
   * The current device activation code.
   */
  protected deviceCode: string | undefined = undefined;

  /**
   * The date since to retrieve the device data.
   */
  protected dateSince: number = 24 * 60 * 60 * 1000;

  protected triggerName: string | undefined = undefined;
  protected triggerPostUrl: string | undefined = undefined;
  protected triggerThreshold: number | undefined = undefined;
  protected triggerParameter: 'humidity' | 'pressure' | 'temperature' | 'gasResistance' | undefined = undefined;
  protected triggerOperator: 'gt' | 'gte' | 'lt' | 'lte' | undefined = undefined;

  /**
   * The current device data.
   */
  private deviceData:
    | {
        humidity: number;
        pressure: number;
        temperature: number;
        gasResistance: number;
        createdOn: Date;
      }[]
    | null = null;

  protected triggers: TriggerInfo[] | null = null;

  private humidityChart: Chart<'line', unknown[], unknown> | null = null;
  private pressureChart: Chart<'line', unknown[], unknown> | null = null;
  private temperatureChart: Chart<'line', unknown[], unknown> | null = null;
  private gasResistanceChart: Chart<'line', unknown[], unknown> | null = null;

  /**
   * Constructor.
   *
   * @param log The log service.
   * @param deviceService The device service.
   * @param triggerService The trigger service.
   * @param dialog The material dialog.
   * @param route The currently active route.
   * @param router The router.
   */
  constructor(
    private log: LogService,
    private deviceService: DeviceService,
    private triggerService: TriggerService,
    private dialog: MatDialog,
    private snackbar: MatSnackBar,
    private route: ActivatedRoute,
    private router: Router,
  ) {
    this.deviceId = this.route.snapshot.paramMap.get('id');
  }

  /**
   * Angular's lifecycle hook for after content initialization.
   */
  public async ngAfterContentInit(): Promise<void> {
    await this.getDeviceInfo();
    await this.getDeviceData();
    await this.getDeviceTriggers();

    this.renderHumidityChart();
    this.createPressureChart();
    this.createTemperatureChart();
    this.createGasResistanceChart();
  }

  /**
   * Generates a new device activation code.
   */
  public async authorizeDevice(): Promise<void> {
    if (!this.deviceId || !this.dialogAuthorize) {
      return;
    }

    try {
      const result = await this.deviceService.createDeviceCode(this.deviceId);
      this.deviceCode = result.code;

      this.dialog.open(this.dialogAuthorize, {
        width: '450px',
      });
    } catch (error) {
      this.log.error(error);
    }
  }

  /**
   * Opens the device deletion dialog.
   */
  public attemptDeleteDevice(): void {
    if (!this.dialogDelete) {
      return;
    }

    this.dialog.open(this.dialogDelete, {
      width: '450px',
    });
  }

  /**
   * Deletes this device.
   */
  public async deleteDevice(): Promise<void> {
    if (!this.deviceId) {
      return;
    }

    try {
      await this.deviceService.deleteDevice(this.deviceId);
      await this.router.navigate(['/devices']);
    } catch (error) {
      this.log.error(error);
    }
  }

  public attemptCreateTrigger(): void {
    if (!this.dialogTrigger) {
      return;
    }

    this.dialog.open(this.dialogTrigger, {
      width: '650px',
    });
  }

  public async createTrigger(): Promise<void> {
    if (
      !this.deviceId ||
      !this.triggerName ||
      !this.triggerPostUrl ||
      !this.triggerThreshold ||
      !this.triggerParameter ||
      !this.triggerOperator
    ) {
      return;
    }

    try {
      await this.triggerService.createTrigger(
        this.deviceId,
        this.triggerName,
        this.triggerPostUrl,
        this.triggerThreshold,
        this.triggerParameter,
        this.triggerOperator,
      );

      await this.getDeviceTriggers();

      this.snackbar.open('Successfully created trigger!', 'Dismiss', {
        duration: 5000,
      });
    } catch (error) {
      this.log.error(error);
      this.snackbar.open(`Failed to create trigger!`, 'Ok', {
        duration: 5000,
      });
    }
  }

  protected async deleteTrigger(triggerId: string): Promise<void> {
    try {
      this.triggerService.deleteTrigger(triggerId);

      this.snackbar.open('Successfully deleted trigger!', 'Ok', {
        duration: 5000,
      });

      await this.getDeviceTriggers();
    } catch (error) {
      this.log.error(error);
      this.snackbar.open(`Failed to delete trigger!`, 'Ok', {
        duration: 5000,
      });
    }
  }

  protected async onSinceSelectionChanged(event: MatSelectChange): Promise<void> {
    const value = event.value as number;
    const since = new Date(Date.now() - value);

    await this.getDeviceData(since);
    this.renderHumidityChart();
    this.createPressureChart();
    this.createTemperatureChart();
    this.createGasResistanceChart();
  }

  /**
   * Renders the humidity chart.
   */
  private renderHumidityChart(): void {
    if (!this.canvasHumidity || !this.deviceData) {
      return;
    }

    this.humidityChart?.destroy();
    this.humidityChart = this.createChart(
      'Humidity',
      this.canvasHumidity.nativeElement,
      this.deviceData.map((point) => point.createdOn),
      this.deviceData.map((point) => point.humidity),
    );
  }

  /**
   * Renders the pressure chart.
   */
  private createPressureChart(): void {
    if (!this.canvasPressure || !this.deviceData) {
      return;
    }

    this.pressureChart?.destroy();
    this.pressureChart = this.createChart(
      'Pressure',
      this.canvasPressure.nativeElement,
      this.deviceData.map((point) => point.createdOn),
      this.deviceData.map((point) => point.pressure),
    );
  }

  /**
   * Renders the temperature chart.
   */
  private createTemperatureChart(): void {
    if (!this.canvasTemperature || !this.deviceData) {
      return;
    }

    this.temperatureChart?.destroy();
    this.temperatureChart = this.createChart(
      'Temperature',
      this.canvasTemperature.nativeElement,
      this.deviceData.map((point) => point.createdOn),
      this.deviceData.map((point) => point.temperature),
    );
  }

  /**
   * Renders the gas resistance chart.
   */
  private createGasResistanceChart(): void {
    if (!this.canvasGasResistance || !this.deviceData) {
      return;
    }

    this.gasResistanceChart?.destroy();
    this.gasResistanceChart = this.createChart(
      'Gas Resistance',
      this.canvasGasResistance.nativeElement,
      this.deviceData.map((point) => point.createdOn),
      this.deviceData.map((point) => point.gasResistance),
    );
  }

  /**
   * Retrieves the device info.
   */
  private async getDeviceInfo(): Promise<void> {
    if (!this.deviceId) {
      return;
    }

    try {
      this.deviceInfo = await this.deviceService.getDevice(this.deviceId);
    } catch (error) {
      this.log.error(error);
    }
  }

  /**
   * Retrieves the device data.
   */
  private async getDeviceData(since = new Date(new Date().getTime() - 24 * 60 * 60 * 1000)): Promise<void> {
    if (!this.deviceId) {
      return;
    }

    try {
      this.deviceData = await this.deviceService.getDeviceData(this.deviceId, since);
    } catch (error) {
      this.log.error(error);
    }
  }

  private async getDeviceTriggers(): Promise<void> {
    if (!this.deviceId) {
      return;
    }

    try {
      this.triggers = await this.triggerService.getTriggers(this.deviceId);
    } catch (error) {
      this.log.error(error);
    }
  }

  protected getEvaluatorText(evaluator: 'lt' | 'lte' | 'gt' | 'gte'): string {
    switch (evaluator) {
      case 'lt':
        return 'Less Than';
      case 'lte':
        return 'Less Than Equals';
      case 'gt':
        return 'Greater Than';
      case 'gte':
        return 'Greater Than Equals';
    }
  }

  /**
   * Renders a chart.
   *
   * @param title The title of the chart.
   * @param canvas The canvas element to render the chart on.
   * @param labels The labels for the chart.
   * @param data The data for the chart.
   */
  private createChart(
    title: string,
    canvas: HTMLCanvasElement,
    labels: unknown[],
    data: unknown[],
  ): Chart<'line', unknown[], unknown> {
    return new Chart(canvas, {
      type: 'line',
      data: {
        labels: labels,
        datasets: [
          {
            label: title,
            data: data,
            borderColor: '#B39DDB',
            fill: true,
          },
        ],
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
          x: {
            type: 'time',
            time: {
              tooltipFormat: 'yyyy-MM-dd HH:mm',
              displayFormats: {
                millisecond: 'HH:mm:ss.SSS',
                second: 'HH:mm:ss',
                minute: 'HH:mm',
                hour: 'HH',
              },
              unit: 'minute',
            },
            title: {
              display: true,
              text: 'Time',
            },
            grid: {
              display: false,
            },
            adapters: {
              date: {
                locale: enUS,
              },
            },
          },
          y: {
            grid: {
              display: false,
            },
          },
        },
        plugins: {
          legend: {
            position: 'bottom',
            align: 'end',
          },
        },
      },
    });
  }
}
