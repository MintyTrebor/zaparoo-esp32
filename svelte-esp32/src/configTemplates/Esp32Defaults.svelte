<script lang="ts">
    import { EspUtils } from "../backend/EspUtils";
    import type { ConfigData } from "../types/ConfigData";
    import { CommonUtils } from "../backend/CommonUtils";
    let config: ConfigData = EspUtils.getBlank();
    EspUtils.config().subscribe(value=>{
      config = value;
      config.audioGain = Math.round(config.audioGain * 100)/100; //Float point number on esp, doesn't matter
    });
    const handleSubmit = (event: Event) => {
      event.preventDefault();
      const update: Partial<ConfigData> = {
        defaultLaunchAudio: CommonUtils.validateAudioPath(config.defaultLaunchAudio),
        defaultRemoveAudio: CommonUtils.validateAudioPath(config.defaultRemoveAudio),
        defaultErrorAudio: CommonUtils.validateAudioPath(config.defaultErrorAudio),
        defaultInsertAudio: CommonUtils.validateAudioPath(config.defaultInsertAudio),
        buzzOnDetect: config.buzzOnDetect,
        buzzOnLaunch: config.buzzOnLaunch,
        buzzOnError: config.buzzOnError,
        buzzOnRemove: config.buzzOnRemove,
        sdCardEnabled: config.sdCardEnabled,
        audioGain: config.audioGain,
        defaultImgPath: CommonUtils.validateAudioPath(config.defaultImgPath)
      };
      EspUtils.updateConfig(update);
    };
</script>
<div class="text-center">
  <h2>ESP32 Defaults</h2>
</div>
<br>
<form on:submit={handleSubmit} class="row g-4">
  {#if config.motorEnabled}
  <div class="col-12">
    <div class="d-flex flex-column flex-md-row align-items-start justify-content-between">
      <div class="input-group-text col-12 col-md-3">
        <div class="form-check form-switch w-100">
          <input class="form-check-input" type="checkbox" role="switch" id="enableBuzzDet" bind:checked={config.buzzOnDetect}/>
          <label class="form-check-label w-100" for="enableBuzzDet">Detect Rumble</label>
        </div>
      </div>
      <div class="input-group-text col-12 col-md-3">
        <div class="form-check form-switch w-100">
          <input class="form-check-input" type="checkbox" role="switch" id="enableBuzzLaun" bind:checked={config.buzzOnLaunch}/>
          <label class="form-check-label w-100" for="enableBuzzLaun">Launch Rumble</label>
        </div>
      </div>
      <div class="input-group-text col-12 col-md-3">
        <div class="form-check form-switch w-100">
          <input class="form-check-input" type="checkbox" role="switch" id="enableBuzzRem" bind:checked={config.buzzOnRemove}/>
          <label class="form-check-label w-100" for="enableBuzzRem">Remove Rumble</label>
        </div>
      </div>
      <div class="input-group-text col-12 col-md-3">
        <div class="form-check form-switch w-100">
          <input class="form-check-input" type="checkbox" role="switch" id="enableBuzzErr" bind:checked={config.buzzOnError}/>
          <label class="form-check-label w-100" for="enableBuzzErr">Error Rumble</label>
        </div>
      </div>
    </div>
  </div>
  {/if}
  {#if config.audioEnabled}
  <div class="col-12">
    <div class="d-flex flex-column flex-md-row align-items-center justify-content-between">
      <div class="form-floating col-12 col-md-3">
        <input type="text" class="form-control" id="defDetPath" placeholder="/" bind:value={config.defaultInsertAudio}/>
        <label for="defDetPath">Detect Audio File</label>
      </div>
      <div class="form-floating col-12 col-md-3">
        <input type="text" class="form-control" id="defLaunPath" placeholder="/" bind:value={config.defaultLaunchAudio}/>
        <label for="defLaunPath">Launch Audio File</label>
      </div>
      <div class="form-floating col-12 col-md-3">
        <input type="text" class="form-control" id="defRemPath" placeholder="/" bind:value={config.defaultRemoveAudio}/>
        <label for="defRemPath">Remove Audio File</label>
      </div>
      <div class="form-floating col-12 col-md-3">
        <input type="text" class="form-control" id="defErrPath" placeholder="/" bind:value={config.defaultErrorAudio}/>
        <label for="defErrPath">Error Audio File</label>
      </div>
    </div>
  </div>
  <div class="col-12">
    <div class="d-flex flex-column flex-md-row justify-content-center">
      <div class="input-group-text col-12 col-md-4">
        <div class="form-floating">
          <input type="number" class="form-control" step="1" id="defAudioGain" placeholder="21" bind:value={config.audioGain}/>
          <label for="defAudioGain">Audio Gain (0 - 21)</label>
        </div>
      </div>
    </div>
  </div>
    {#if config.deviceType == "Lilygo"}
    <div class="col-12">
      <div class="d-flex flex-column flex-md-row justify-content-center">
        <div class="input-group-text col-12 col-md-4">
          <div class="form-floating">
            <input type="text" class="form-control" id="defImgPath" placeholder="/" bind:value={config.defaultImgPath}/>
            <label for="defAudioGain">Default Image Path</label>
          </div>
        </div>
      </div>
    </div>
    {/if}

  {/if}
  {#if config.motorEnabled || config.audioEnabled}
  <button type="submit" class="btn btn-primary mt-3">Save</button>
  {/if}
  {#if !config.motorEnabled && !config.audioEnabled}
    <div class="text-center mt-5">
      <h3>Both Audio & Rumble Motor are disabled in ESP32 Config</h3>
      <h3>Enable to change default settings.</h3>
    </div>
  {/if}
</form>
