<script lang="ts">
  import { InstallButton } from 'esp-web-tools';

  let installButton: InstallButton;
  let foundFirmware = false;
  let firmwareUrl: string | null = null;

  function attachInstallButton() {
    const container = document.getElementById('install-button-container');
    if (container) {
      container.innerHTML = '';
      installButton = new InstallButton();
      installButton.logConsole = true;
      installButton.showLog = true;
      installButton.eraseFirst = true;
      container.appendChild(installButton);
    }
  }

  function handleFileUpload(event: Event) {
    const input = event.target as HTMLInputElement;
    if (input.files && input.files.length > 0) {
      const file = input.files[0];
      firmwareUrl = URL.createObjectURL(file);
      const manifestContents = {
        "name": "Zaparoo ESP32",
        "builds": [
          {
            "chipFamily": "ESP32-S3",
            "parts": [
              {
                "path": firmwareUrl,
                "offset": 0
              }
            ]
          }
        ]
      };
      const manifestBlob = new Blob([JSON.stringify(manifestContents, null, 2)], { type: "application/json" });
      const manifestUrl = URL.createObjectURL(manifestBlob);
      attachInstallButton();
      installButton.manifest = manifestUrl;
      foundFirmware = true;
    }
  }
</script>

<main class="container-sm mt-5 text-light bg-dark p-4 rounded shadow-lg" data-bs-theme="dark">
  <div class="text-center">
    <h1 class="mb-4">Zaparoo ESP32 Installer</h1>
    <div class="mb-3">
      <label for="firmware-upload" class="form-label fw-bold d-flex align-items-center">Select Firmware File:</label>
      <input id="firmware-upload" type="file" accept=".bin" class="form-control form-control-sm w-auto border-0" on:change={handleFileUpload} />
    </div>
    <div id="install-button-container" class="mt-4"></div>
  </div>
</main>
