<script lang="ts">
    import { EspUtils } from "../backend/EspUtils";
    import type { ConfigData } from "../types/ConfigData";
    let config: ConfigData = EspUtils.getBlank();
    EspUtils.config().subscribe(value=> config = value);
    const handleSubmit = (event: Event) => {
      event.preventDefault();
      const update: Partial<ConfigData> = {
          wifiLedEnabled: config.wifiLedEnabled,
          motorEnabled: config.motorEnabled,
          launchLedEnabled: config.launchLedEnabled,
          audioEnabled: config.audioEnabled,
          pwrLedEnabled: config.pwrLedEnabled,
          i2sDoutPin: config.i2sDoutPin,
          i2sBclkPin: config.i2sBclkPin,
          i2sLrcPin: config.i2sLrcPin,
          motorPin: config.motorPin,
          wifiLedPin: config.wifiLedPin,
          launchLedPin: config.launchLedPin,
          pwrLedPin: config.pwrLedPin,
          sdCardEnabled: config.sdCardEnabled
      };
      EspUtils.updateConfig(update);
    };
</script>
{#if config.deviceType != "Lilygo"}
<form on:submit={handleSubmit} class="row g-4">
  <div class="col-12 col-md-6">
    <div class="input-group">
      <div class="input-group-text">
        <div class="form-check form-switch">
          <input class="form-check-input" type="checkbox" role="switch" id="enableWiFiLED" bind:checked={config.wifiLedEnabled}/>
          <label class="form-check-label" for="enableWiFiLED">WiFi LED</label>
        </div>
      </div>
      <div class="form-floating">
        <input type="number" class="form-control" id="pinWifiLED" placeholder="2" bind:value={config.wifiLedPin} disabled={!config.wifiLedEnabled} />
        <label for="pinWifiLED">GPIO PIN</label>
      </div>
    </div>
  </div>
  <div class="col-12 col-md-6">
    <div class="input-group">
      <div class="input-group-text">
        <div class="form-check form-switch">
          <input class="form-check-input" type="checkbox" role="switch" id="enableLaunchLED" bind:checked={config.launchLedEnabled}/>
          <label class="form-check-label" for="enableLaunchLED">Launch LED</label>
        </div>
      </div>
      <div class="form-floating">
        <input type="number" class="form-control" id="pinLaunchLED" placeholder="33" bind:value={config.launchLedPin} disabled={!config.launchLedEnabled} />
        <label for="pinLaunchLED">GPIO PIN</label>
      </div>
    </div>
  </div>
  <div class="col-12 col-md-6">
    <div class="input-group">
      <div class="input-group-text">
        <div class="form-check form-switch">
          <input class="form-check-input" type="checkbox" role="switch" id="enablePwrLED" bind:checked={config.pwrLedEnabled}/>
          <label class="form-check-label" for="enablePwrLED">Power LED</label>
        </div>
      </div>
      <div class="form-floating">
        <input type="number" class="form-control" id="pinPwrLED" placeholder="15" bind:value={config.pwrLedPin} disabled={!config.pwrLedEnabled} />
        <label for="pinPwrLED">GPIO PIN</label>
      </div>
    </div>
  </div>
  <div class="col-12 col-md-6">
    <div class="input-group">
      <div class="input-group-text">
        <div class="form-check form-switch">
          <input class="form-check-input" type="checkbox" role="switch" id="enableRumbleLED" bind:checked={config.motorEnabled}/>
          <label class="form-check-label" for="enableRumbleLED">Rumble Motor</label>
        </div>
      </div>
      <div class="form-floating">
        <input type="number" class="form-control" id="pinMotor" placeholder="32" bind:value={config.motorPin} disabled={!config.motorEnabled} />
        <label for="pinMotor">GPIO PIN</label>
      </div>
    </div>
  </div>
  <div class="col-12">
    <div class="input-group">
      <div class="input-group-text col-md-4 col-12">
        <div class="form-check form-switch">
          <input class="form-check-input" type="checkbox" role="switch" id="enableAudio" bind:checked={config.audioEnabled}/>
          <label class="form-check-label" for="enableAudio">Audio Module</label>
        </div>
      </div>
      <div class="form-floating col-md-2 col-12">
        <input type="number" class="form-control" id="pinI2SDOUT" placeholder="25" bind:value={config.i2sDoutPin} disabled={!config.audioEnabled} />
        <label for="pinI2SDOUT">GPIO I2SDOUT</label>
      </div>
      <div class="form-floating col-md-2 col-12">
        <input type="number" class="form-control" id="pinI2SBCLK" placeholder="27" bind:value={config.i2sBclkPin} disabled={!config.audioEnabled} />
        <label for="pinI2SBCLK">GPIO I2SBCLK</label>
      </div>
      <div class="form-floating col-md-2 col-12">
        <input type="number" class="form-control" id="pinI2SLRC" placeholder="26" bind:value={config.i2sLrcPin} disabled={!config.audioEnabled} />
        <label for="pinI2SLRC">GPIO I2SLRC</label>
      </div>
    </div>
  </div>
  <div class="col-12">
    <div class="input-group justify-content-md-center">
      <div class="input-group-text">
        <div class="form-check form-switch">
          <input class="form-check-input" type="checkbox" role="switch" id="enableSDCard" bind:checked={config.sdCardEnabled}/>
          <label class="form-check-label" for="enableSDCard">SD Card Module</label>
        </div>
      </div>
    </div>
  </div>
  <button type="submit" class="btn btn-primary mt-3">Save</button>
</form>
{/if}
{#if config.deviceType == "Lilygo"}
<form on:submit={handleSubmit} class="row g-3">
  <div class="col-12">
    <div class="input-group">
      <div class="input-group-text col-3">
        <div class="form-check form-switch">
          <input class="form-check-input" type="checkbox" role="switch" id="enableLaunchLED" bind:checked={config.launchLedEnabled}/>
          <label class="form-check-label" for="enableLaunchLED">Launch LED</label>
        </div>
      </div>
      <div class="input-group-text  col-3">
        <div class="form-check form-switch">
          <input class="form-check-input" type="checkbox" role="switch" id="enablePwrLED" bind:checked={config.pwrLedEnabled}/>
          <label class="form-check-label" for="enablePwrLED">Power LED</label>
        </div>
      </div>
      <div class="input-group-text  col-3">
        <div class="form-check form-switch">
          <input class="form-check-input" type="checkbox" role="switch" id="enableRumbleLED" bind:checked={config.motorEnabled}/>
          <label class="form-check-label" for="enableRumbleLED">Rumble Motor</label>
        </div>
      </div>
      <div class="form-floating col-3">
        <input type="number" class="form-control" id="pinMotor" placeholder="32" bind:value={config.motorPin} disabled={!config.motorEnabled} />
        <label for="pinMotor">GPIO PIN</label>
      </div>
    </div>
  </div>
  <div class="col-12">
    <div class="input-group col-6">
      <div class="input-group-text col-6">
        <div class="form-check form-switch">
          <input class="form-check-input" type="checkbox" role="switch" id="enableAudio" bind:checked={config.audioEnabled}/>
          <label class="form-check-label" for="enableAudio">Audio Module</label>
        </div>
      </div>
      <div class="input-group-text col-6">
        <div class="form-check form-switch">
          <input class="form-check-input" type="checkbox" role="switch" id="enableSDCard" bind:checked={config.sdCardEnabled}/>
          <label class="form-check-label" for="enableSDCard">SD Card Module</label>
        </div>
      </div>
    </div>
  </div>
  <button type="submit" class="btn btn-primary mt-3">Save</button>
</form>
{/if}

