#pragma once

constexpr char dashboardHtml[] = R"HTML(

<h1>DriveWire</h1>

<button id="forward" type="button" style="touch-action: none">Forward</button>
<button id="stop" type="button">Stop</button>

<script>
  const forward = document.getElementById('forward');
  let repeatTimer = null;
  let pendingForward = null;
  let stopping = false;

  async function sendForward() {
    if (pendingForward) return;

    pendingForward = fetch('/test-forward', { method: 'POST' });
    try {
      await pendingForward;
    } catch (error) {
      console.error(error);
    } finally {
      pendingForward = null;
    }
  }

  async function stopDriving() {
    if (stopping) return;
    stopping = true;
    clearInterval(repeatTimer);
    repeatTimer = null;

    try {
      if (pendingForward) {
        await pendingForward.catch(() => {});
      }
      await fetch('/stop', { method: 'POST' });
    } catch (error) {
      console.error(error);
    } finally {
      stopping = false;
    }
  }

  forward.addEventListener('pointerdown', (event) => {
    if (repeatTimer !== null || stopping || event.button !== 0) return;

    forward.setPointerCapture(event.pointerId);
    sendForward();
    repeatTimer = setInterval(sendForward, 100);
  });

  forward.addEventListener('pointerup', stopDriving);
  forward.addEventListener('pointercancel', stopDriving);
  document.getElementById('stop').addEventListener('click', stopDriving);

  window.addEventListener('blur', () => {
    if (repeatTimer !== null) stopDriving();
  });
</script>









)HTML";