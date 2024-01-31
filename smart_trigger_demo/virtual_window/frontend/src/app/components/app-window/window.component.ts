import { CommonModule } from '@angular/common';
import { AfterContentInit, Component, OnDestroy } from '@angular/core';
import { MatButtonModule } from '@angular/material/button';
import { MatCardModule } from '@angular/material/card';
import { MatIconModule } from '@angular/material/icon';
import { MatSidenavModule } from '@angular/material/sidenav';
import { MatToolbarModule } from '@angular/material/toolbar';
import { RouterOutlet } from '@angular/router';
import { environment } from '../../../environments/environment';

@Component({
  selector: 'app-window',
  standalone: true,
  imports: [
    CommonModule,
    RouterOutlet,
    MatButtonModule,
    MatToolbarModule,
    MatIconModule,
    MatCardModule,
    MatSidenavModule,
  ],
  templateUrl: './window.component.html',
  styleUrl: './window.component.scss',
})
export class WindowComponent implements AfterContentInit, OnDestroy {
  protected windowOpenLeft = false;
  protected windowOpenRight = false;
  protected windowCloseLeft = false;
  protected windowCloseRight = false;

  private webSocket: WebSocket | null = null;

  public ngAfterContentInit(): void {
    this.webSocket = new WebSocket(environment.api);

    this.webSocket.addEventListener('open', () => {
      console.info('[ws] connection established');
    });

    this.webSocket.addEventListener('message', (event) => {
      console.info('[ws] message received:', event.data);

      const data = JSON.parse(event.data) as { windowOpened: boolean };

      if (data.windowOpened) {
        this.windowOpenLeft = true;
        this.windowOpenRight = true;
        this.windowCloseLeft = false;
        this.windowCloseRight = false;
      } else {
        this.windowOpenLeft = false;
        this.windowOpenRight = false;
        this.windowCloseLeft = true;
        this.windowCloseRight = true;
      }
    });
  }

  public ngOnDestroy(): void {
    this.webSocket?.close();
  }
}
