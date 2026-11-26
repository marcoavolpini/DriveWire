#pragma once

constexpr char dashboardHtml[] = R"HTML(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>DriveWire</title>

  <style>
    * {
      -webkit-user-select: none;
      user-select: none;
      -webkit-touch-callout: none;
      -webkit-tap-highlight-color: transparent;
    }

    body {
      margin: 0;
      font-family: system-ui, sans-serif;
    }

    main {
      box-sizing: border-box;
      width: 100%;
      max-width: 42rem;
      min-height: 100vh;
      min-height: 100dvh;
      margin: 0 auto;
      padding: 0.75rem;
      display: grid;
      grid-template-rows: auto auto 1fr;
      gap: 0.75rem;
    }

    h1 {
      margin: 0;
      font-size: clamp(1.5rem, 6vw, 2rem);
    }

    button {
      padding: 0.5rem;
      font: inherit;
      font-size: clamp(1rem, 4.5vw, 1.25rem);
      font-weight: 600;
    }

    #boost {
      min-height: 3rem;
      touch-action: manipulation;
    }

    #boost[aria-pressed="true"] {
      background: #f59e0b;
      color: #171717;
    }

    .drive-pad {
      display: grid;
      grid-template-areas:
        "forward-left forward forward-right"
        "left stop right"
        "reverse-left reverse reverse-right";
      grid-template-columns: repeat(3, minmax(0, 1fr));
      grid-template-rows: repeat(3, minmax(3.5rem, 1fr));
      gap: 0.5rem;
    }

    .drive-pad button {
      touch-action: none;
    }
  </style>
</head>

<body>
  <main>
    <h1>DriveWire</h1>
    <button id="boost" type="button" aria-pressed="false">Boost off</button>

    <div class="drive-pad">
      <button type="button" style="grid-area: forward-left" data-route="/forward-left">Forward Left</button>
      <button type="button" style="grid-area: forward" data-route="/forward">Forward</button>
      <button type="button" style="grid-area: forward-right" data-route="/forward-right">Forward Right</button>
      <button type="button" style="grid-area: left" data-route="/pivot-left">Pivot Left</button>
      <button type="button" style="grid-area: stop" id="stop">Stop</button>
      <button type="button" style="grid-area: right" data-route="/pivot-right">Pivot Right</button>
      <button type="button" style="grid-area: reverse-left" data-route="/reverse-left">Reverse Left</button>
      <button type="button" style="grid-area: reverse" data-route="/reverse">Reverse</button>
      <button type="button" style="grid-area: reverse-right" data-route="/reverse-right">Reverse Right</button>
    </div>
  </main>

  <script>
    let activePointer = null;
    let repeatTimer = null;
    let pendingDrive = null;
    let stopping = false;

    function sendDrive(route) {
      if (pendingDrive) return;

      pendingDrive = fetch(route, { method: 'POST' })
        .catch(error => console.error(error))
        .finally(() => {
          pendingDrive = null;
        });
    }

    async function stopDriving() {
      if (stopping) return;

      stopping = true;
      activePointer = null;
      clearInterval(repeatTimer);

      try {
        await pendingDrive;
        await fetch('/stop', { method: 'POST' });
      } catch (error) {
        console.error(error);
      } finally {
        stopping = false;
      }
    }

    function releaseDirection(event) {
      if (event.pointerId === activePointer) stopDriving();
    }

    for (const button of document.querySelectorAll('[data-route]')) {
      const route = button.dataset.route;

      button.addEventListener('pointerdown', event => {
        if (activePointer !== null || stopping || event.button !== 0) return;

        button.setPointerCapture(event.pointerId);
        activePointer = event.pointerId;
        sendDrive(route);
        repeatTimer = setInterval(() => sendDrive(route), 100);
      });

      button.addEventListener('pointerup', releaseDirection);
      button.addEventListener('pointercancel', releaseDirection);
      button.addEventListener('lostpointercapture', releaseDirection);
    }

    document.getElementById('stop').addEventListener('click', stopDriving);

    window.addEventListener('blur', () => {
      if (activePointer !== null) stopDriving();
    });

    const boostButton = document.getElementById('boost');

    async function toggleBoost() {
      if (boostButton.disabled) return;

      const enabled = boostButton.getAttribute('aria-pressed') !== 'true';
      boostButton.disabled = true;

      try {
        const response = await fetch(`/boost?enabled=${enabled ? 1 : 0}`, {
          method: 'POST'
        });

        if (!response.ok) {
          throw new Error(`Boost request failed (${response.status})`);
        }

        boostButton.setAttribute('aria-pressed', String(enabled));
        boostButton.textContent = enabled ? 'Boost armed' : 'Boost off';
      } catch (error) {
        boostButton.textContent = 'Boost request failed';
        console.error(error);
      } finally {
        boostButton.disabled = false;
      }
    }

    boostButton.addEventListener('pointerup', event => {
      if (event.button === 0) toggleBoost();
    });

    boostButton.addEventListener('click', event => {
      if (event.detail === 0) toggleBoost();
    });
  </script>
</body>
</html>
)HTML";