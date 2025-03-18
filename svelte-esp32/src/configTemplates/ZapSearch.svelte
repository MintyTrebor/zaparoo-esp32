<script lang="ts">
  import type { zapSystems, zapSearchResults, htmlFormattedSearchRes, sourceZapSvsList, ConfigData } from '../types/ConfigData';
  import { ZapUtils } from '../backend/ZapUtils';
  import { EspUtils } from "../backend/EspUtils";
  export let searchReturn: any;
  let zapSysList: zapSystems = ZapUtils.getBlankSystems();
  let zapSvsList: sourceZapSvsList = ZapUtils.getActiveSourceList();
  let zapSrchRes: zapSearchResults = ZapUtils.getBlankSearchResults();
  let htmlSerRes: htmlFormattedSearchRes = ZapUtils.getBlankhmtlSrchRes();
  let selectedSource: string = "";
  let config: ConfigData = EspUtils.getBlank();
  EspUtils.config().subscribe(value=> config = value);
  ZapUtils.zapSrcRes().subscribe(value=> zapSrchRes = value);
  ZapUtils.indexedSystemsList().subscribe(value=> zapSysList = value);
  ZapUtils.htmlSrchRes().subscribe(value=> htmlSerRes = value);
  EspUtils.config().subscribe(value=> settingsIsChanged(value));
  let selectedSys: string = "*";
  let searchQry: string | null;
  let selectedGame: string = "";
  let writeTxtHelp: string = "Write the Game Launch & Audio File Paths to NFC Card/Tag";
  let testLaunchTxtHelp: string = "Test Launch the selected Game"
  

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

  function doTestLaunch() {
    ZapUtils.doTestLaunch(selectedGame);
  };

  function doDBIndex() {
    ZapUtils.updateGamesDB();
  }

  function acceptResults(){
    searchReturn?.({selectedGame});
  }

    
</script>
<div class="text-center mb-3">
  <h2>Search For Game</h2>
</div>
<form on:submit={handleSubmit} class="row g-3">
  <div class="col-12">
    <div class="input-group mb-3">
      <div class="col-5">
        <label for="selSystem">Select Source</label><select class="form-select" id="selSystem" bind:value={selectedSource} on:change={getSystems}>
          {#each zapSvsList.sources as { value, name}}
            {#if value != 'manual'}  
              <option value={value}>{name}</option>
            {/if}
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
      {/if}
    {/if}
    {#if selectedGame.length > 0}
    <div class="container">
      <div class="row">
        <div class="col text-center">  
          <button type="button" class="btn btn-primary mt-4" on:click="{acceptResults}" data-bs-toggle="tooltip" title="{writeTxtHelp}" data-bs-placement="top">OK</button>
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
