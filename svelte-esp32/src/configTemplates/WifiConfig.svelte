<script lang="ts">
  import { EspUtils } from "../backend/EspUtils";
    import { LogUtils } from "../backend/LogUtils";
  let wifi = "";
  let password = "";
  let file: File | null = null;
  let uploadProgress = 0;


  const handleSubmit = (event: Event) => {
      event.preventDefault();
      EspUtils.updateWifi(wifi, password);
  };

  const handleFileChange = (event: Event) => {
      const target = event.target as HTMLInputElement;
      if (target.files && target.files.length > 0) {
          file = target.files[0];
      }
  };
  const handleUpload = async () => {
    let uploadStatus = "Upload complete! Page will reload";
    let completed = true;
    if (!file) {
        uploadStatus = "Please select a firmware file.";
        return;
    }

    uploadProgress = 0;
    try {
        await EspUtils.uploadFirmware(file, (progress) => {
            uploadProgress = progress;
        });
    } catch (error) {
        uploadStatus = "Upload failed. Please try again.";
        completed = false;
    }
    LogUtils.notify(uploadStatus);
    if(completed){
      setTimeout(()=>{
        location.reload();
      }, 6000);
  }
};
</script>

<div class="text-center">
  <h2>Wi-Fi Configuration</h2>
</div>
<form on:submit={handleSubmit} class="row g-2">
  <div class="mb-3">
      <label for="ssid" class="form-label">Network</label>
      <input bind:value={wifi} type="text" class="form-control" id="ssid" placeholder="Wi-Fi Network">
  </div>
  <div class="mb-3">
      <label for="password" class="form-label">Password</label>
      <input bind:value={password} type="password" class="form-control" id="password">
  </div>
  <button type="submit" class="btn btn-primary mt-3">Save</button>
</form>

<hr>
<!--Firmware-->
<div class="text-center mt-4">
  <h2>Firmware Update</h2>
</div>
<div class="mb-3">
  <input type="file" class="form-control" accept=".bin" on:change={handleFileChange} />
</div>
<button on:click={handleUpload} class="btn btn-success mt-2">Upload Firmware</button>
{#if uploadProgress > 0}
  <div class="progress mt-3">
      <div class="progress-bar" role="progressbar" style="width: {uploadProgress}%;">
          {uploadProgress}%
      </div>
  </div>
{/if}
