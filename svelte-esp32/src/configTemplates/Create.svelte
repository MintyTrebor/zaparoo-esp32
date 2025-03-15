<script lang="ts">
  import type { zapSystems, zapSearchResults, htmlFormattedSearchRes, writeResultState, sourceZapSvsList, ConfigData, UIDExtdRecord, UIDFileJson } from '../types/ConfigData';
  import { ZapUtils } from '../backend/ZapUtils';
  import { CommonUtils } from '../backend/CommonUtils';
  import { EspUtils } from "../backend/EspUtils";
  import { UIDUtils } from '../backend/UIDUtils';
  let zapSysList: zapSystems = ZapUtils.getBlankSystems();
  let zapSvsList: sourceZapSvsList = ZapUtils.getActiveSourceList();
  let zapSrchRes: zapSearchResults = ZapUtils.getBlankSearchResults();
  let htmlSerRes: htmlFormattedSearchRes = ZapUtils.getBlankhmtlSrchRes();
  let selectedSource: string = "";
  let dialogWait: HTMLDialogElement;
  let uidRecord: UIDExtdRecord = UIDUtils.getBlank();
  let lastUIDVal : string = "";
  let config: ConfigData = EspUtils.getBlank();
  let uidFJson: UIDFileJson = UIDUtils.getBlankFileJson();
  UIDUtils.scannedUIDFileJson().subscribe(value=> tempUID(value));
  EspUtils.config().subscribe(value=> config = value);
  //UIDUtils.UIDRecord().subscribe(value=> tempUID(value));
  ZapUtils.zapSrcRes().subscribe(value=> zapSrchRes = value);
  ZapUtils.indexedSystemsList().subscribe(value=> zapSysList = value);
  ZapUtils.htmlSrchRes().subscribe(value=> htmlSerRes = value);
  ZapUtils.writeResStat().subscribe(value => WrStIsChanged(value));
  EspUtils.config().subscribe(value=> settingsIsChanged(value));
  let selectedSys: string = "*";
  let searchQry: string | null;
  let selectedGame: string = "";
  let audLaunchP: string | null ;
  let audRemoveP: string | null ;
  let picLaunchP: string | null ;
  let dialogWrite: HTMLDialogElement;
  let dialogSuccess: HTMLDialogElement;
  let dialogFailure: HTMLDialogElement;
  let dialogNoCard: HTMLDialogElement;
  let dialogScriptStart: HTMLDialogElement;
  let mapTxtHelp: string = "Create a map between a readonly RFID Tag/Amiibo & the selected game Launch Path. Mapping Record is stored in Zaparoo.";
  let writeTxtHelp: string = "Write the Game Launch & Audio File Paths to NFC Card/Tag";
  let testLaunchTxtHelp: string = "Test Launch the selected Game"
  
     
  function WrStIsChanged(currVal: writeResultState) {
      switch(currVal.state){
        case 1:
          diagWaitClose();
          diagSucessOpen();
          break;
        case 2:
          diagWaitClose();
          diagFailOpen();
          break;
        case 3:
          diagWaitClose();
          diagNoCardOpen();
          break;
      }
  }

  // function tempUID(currUIDrec: UIDExtdRecord){
  //   if(lastUIDVal != currUIDrec.UID){
  //     lastUIDVal = currUIDrec.UID;
  //   }
  // }

  function tempUID(currUIDrec: UIDFileJson){
    if(lastUIDVal != UIDUtils.currScannedUID()){
      lastUIDVal = UIDUtils.currScannedUID();
    }
  }

  function settingsIsChanged(currSet: ConfigData){
    zapSvsList = ZapUtils.getActiveSourceList();
  }

  function getSystems(){
    zapSysList = ZapUtils.getBlankSystems();
    ZapUtils.initConnection(selectedSource);
    searchQry = "";
    selectedGame = "";
  }
  
  const handleSubmit = (event: Event) => {
    event.preventDefault();
    ZapUtils.doSearch(selectedSys, searchQry);
  };

  function doWrite() {
    if(audLaunchP){audLaunchP = CommonUtils.validateAudioPath(audLaunchP)};
    if(audRemoveP){audRemoveP = CommonUtils.validateAudioPath(audRemoveP)};
    if(picLaunchP){picLaunchP = CommonUtils.validateAudioPath(picLaunchP)};
    ZapUtils.doWriteCard(selectedGame, audLaunchP, audRemoveP, picLaunchP);
    diagWriteClose();
    diagWaitOpen();
  };

  function diagScriptStart(){
    UIDUtils.setUIDMode(true);
    lastUIDVal = "";
    dialogScriptStart.showModal();
  }

  function diagWriteClose() {
    dialogWrite.close("true");
  }

  function diagWriteCancel() {
    dialogWrite.close("true");
    ZapUtils.toggleCreateMode(false);
  }

  function diagWriteOpen() {
    dialogWrite.showModal();
    ZapUtils.toggleCreateMode(true);
  }

  function diagWaitOpen(){
    dialogWait.showModal();
  }

  function diagWaitClose(){
    dialogWait.close("true");
  }

  function diagNoCardOpen(){
    dialogNoCard.showModal();
  }

  function diagNoCardClose(){
    dialogNoCard.close("true");
    ZapUtils.toggleCreateMode(false);
  }

  function diagFailOpen(){
    dialogFailure.showModal();
  }

  function diagFailClose(){
    dialogFailure.close("true");
    ZapUtils.toggleCreateMode(false);
  }

  function diagSuccessClose() {
    dialogSuccess.close("true");
    ZapUtils.toggleCreateMode(false);
  }

  function diagSucessOpen() {
    dialogSuccess.showModal();
  }

  function doTestLaunch() {
    ZapUtils.doTestLaunch(selectedGame);
  };

  function doDBIndex() {
    ZapUtils.updateGamesDB();
  }

  function diagWriteMapOK() {
    if(audLaunchP){audLaunchP = CommonUtils.validateAudioPath(audLaunchP)};
    if(audRemoveP){audRemoveP = CommonUtils.validateAudioPath(audRemoveP)};
    if(picLaunchP){picLaunchP = CommonUtils.validateAudioPath(picLaunchP)};
    ZapUtils.doWriteZapScript(lastUIDVal, selectedGame, audLaunchP, audRemoveP, picLaunchP);
    dialogScriptStart.close("true");
    UIDUtils.setUIDMode(false);
  }

  function diagWriteMapCancel() {
    dialogScriptStart.close("true");
    UIDUtils.setUIDMode(false);
    //uidRecord = UIDUtils.getBlank();
   }
    
</script>
<div class="text-center mb-3">
  <h2>Create Tag</h2>
</div>
<form on:submit={handleSubmit} class="row g-3">
  <div class="col-12">
    <div class="input-group mb-3">
      <div class="col-5">
        <label for="selSystem">Select Source</label><select class="form-select" id="selSystem" bind:value={selectedSource} on:change={getSystems}>
          {#each zapSvsList.sources as { value, name}}
            <option value={value}>{name}</option>
          {/each}
        </select>
      </div>
      <div class="col-1"></div>
      {#if zapSysList.systems && selectedSource != ""}
      <div class="col-2">
        <button type="button" class="btn btn-primary mt-4" on:click={doDBIndex}>Update Zap DB</button>
      </div>
      {/if}
    </div>
    {#if zapSysList.systems && selectedSource != ""}
    <div class="input-group ">
      <div class="col-5">
        <label for="selSystem">Select System</label><select class="form-select" id="selSystem" bind:value={selectedSys}>
          {#each zapSysList.systems as { id, name}}
            <option value={id}>{name}</option>
          {/each}
        </select>
      </div>
      <div class="col-6">
        <label for="searchQry">Search Query</label>
        <input type="text" class="form-control" id="searchQry" bind:value={searchQry}/>
      </div>
      <div class="col-1">
        <button type="submit" class="btn btn-primary mt-4">Search</button>
      </div>
    </div>
    {/if}
    {#if selectedSource === "manual"}
    <div class="input-group mt-3">
      <div class="col-12">
        <label for="manualText">Text</label><input class="form-control" id="manualText" type="text" bind:value={selectedGame}/>
      </div>
    </div>
      <div class="input-group mt-3">
        <div class="col-6">
          <label for="searchQry">Launch Audio Path</label>
          <input type="text" class="form-control" id="aLauchP" bind:value={audLaunchP}/>
        </div>
        <div class="col-6">
          <label for="searchQry">Remove Audio Path</label>
          <input type="text" class="form-control" id="aRemoveP" bind:value={audRemoveP}/>
        </div>
      </div>
    {/if}
    {#if !zapSysList.systems && selectedSource === "zaparoo"}
    <div class="text-center mt-5">
      <h3>Unable to connect to Zaparoo service. Check the service is running and IP address is correct.</h3>
    </div>
    <div class="text-center mt-5">
      <h3>Reload this window to try again</h3>
    </div>
    {/if}
    {#if zapSrchRes.total > 0}
      <div class="input-group mt-3">
        <div class="col-8">
          <label for="selSystem">Search Results ({zapSrchRes.total < 250 ? zapSrchRes.total:'250'}) - Select a Game to write to Card</label><select class="form-select" id="selGame" bind:value={selectedGame}>
            {#each htmlSerRes.results as {name, path}}
              <option value={path}>{name}</option>
            {/each}
          </select>
        </div>
      </div>
      {#if selectedGame.length > 0}
        <div class="input-group mt-3">
          <div class="col-12">
            <label for="searchQry">Launch Path</label>
            <input type="text" class="form-control" id="txtLauchP" disabled bind:value={selectedGame}/>
          </div>
        </div>
        <div class="input-group mt-3">
          {#if config.audioEnabled}
          <div class="col-4">
            <label for="searchQry">Launch Audio Path</label>
            <input type="text" class="form-control" id="aLauchP" bind:value={audLaunchP}/>
          </div>
          <div class="col-4">
            <label for="searchQry">Remove Audio Path</label>
            <input type="text" class="form-control" id="aRemoveP" bind:value={audRemoveP}/>
          </div>
          {/if}
          {#if config.deviceType == "Lilygo"}
          <div class="col-4">
            <label for="searchQry">JPEG Path</label>
            <input type="text" class="form-control" id="aRemoveP" bind:value={picLaunchP}/>
          </div>
          {/if}
        </div>
      {/if}
    {/if}
    {#if selectedGame.length > 0}
    <div class="container">
      <div class="row">
        <div class="col text-center">  
          <button type="button" class="btn btn-primary mt-4" on:click="{diagWriteOpen}" data-bs-toggle="tooltip" title="{writeTxtHelp}" data-bs-placement="top">Write To Card</button>
          <button type="button" class="btn btn-primary mt-4" on:click="{diagScriptStart}" data-bs-toggle="tooltip" title="{mapTxtHelp}" data-bs-placement="top">Create Zaparoo Mapping</button>        
          <button type="button" class="btn btn-primary mt-4" on:click="{doTestLaunch}" data-bs-toggle="tooltip" title="{testLaunchTxtHelp}" data-bs-placement="top">Test Launch</button>
        </div>
      </div>
    </div>
    {/if}
    {#if zapSrchRes.total == 0}
    <div class="text-center mt-3">
      <h2>No Results</h2>
    </div>
    {/if}
  </div>
</form>
<dialog bind:this={dialogWrite}>
  <div class="text-center mb-3">
    <h4>Hold/Insert Card into reader and click OK to write data</h4>
  </div>
  <div class="container">
    <div class="row">
      <div class="col text-center">  
        <button type="button" class="btn btn-primary mt-4" on:click="{doWrite}">OK</button>      
        <button type="button" class="btn btn-primary mt-4" on:click="{diagWriteCancel}">Cancel</button>
      </div>
    </div>
  </div>
</dialog>
<dialog bind:this={dialogSuccess}>
  <div class="text-center mb-3">
    <h4>Remove Card and click OK to continue</h4>
  </div>
  <div class="container">
    <div class="row">
      <div class="col text-center">  
        <button type="button" class="btn btn-primary mt-4" on:click="{diagSuccessClose}">OK</button>      
      </div>
    </div>
  </div>
</dialog>
<dialog bind:this={dialogFailure}>
  <div class="text-center mb-3">
    <h4>Write Failed! Check TAG/Card type. Remove Card -> Click OK & try again</h4>
  </div>
  <div class="container">
    <div class="row">
      <div class="col text-center">  
        <button type="button" class="btn btn-primary mt-4" on:click="{diagFailClose}">OK</button>      
      </div>
    </div>
  </div>
</dialog>
<dialog bind:this={dialogNoCard}>
  <div class="text-center mb-3">
    <h4>Write Failed! Card Not Detected -> Click OK & try again</h4>
  </div>
  <div class="container">
    <div class="row">
      <div class="col text-center">  
        <button type="button" class="btn btn-primary mt-4" on:click="{diagNoCardClose}">OK</button>      
      </div>
    </div>
  </div>
</dialog>
<dialog bind:this={dialogWait}>
  <div class="text-center mb-3">
    <h4>Writing Data To Card - Please Wait</h4>
  </div>
</dialog>
<dialog bind:this={dialogScriptStart}>
  <div class="text-center mb-3">
    <h4>Scan Read Only NFC Tag/Amiibo</h4>
    {#if lastUIDVal != ""}
    <h4>Tag UID: {lastUIDVal}</h4>
    <h4>Click OK to Set Mapping to Current Game & Save Audio File Paths</h4>
    <h4>Clicking OK will overwrite any existing Mapping & Paths!</h4>
    <button type="button" class="btn btn-primary mt-4" on:click="{diagWriteMapOK}">OK</button>
    {/if}
    <button type="button" class="btn btn-primary mt-4" on:click="{diagWriteMapCancel}">Cancel</button>
  </div>
</dialog>