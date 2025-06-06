<style>
  .srchDialog {
    min-height: 70vh;
    min-width: 70vw;
  }
  .colDialog {
    min-height: 200px;
    min-width: 200px;
  }
  .dark {
    --cp-bg-color: #333;
    --cp-border-color: white;
    --cp-text-color: white;
    --cp-input-color: #555;
    --cp-button-hover-color: #777;
  }
</style>
<script lang="ts">
  import type {  zapScriptCmd } from "../types/ConfigData";
  import { UIDUtils } from "../backend/UIDUtils";
  import { FontAwesomeIcon } from '@fortawesome/svelte-fontawesome';
  import SearchDialog from './ZapSearch.svelte';
  import ColorPicker from 'svelte-awesome-color-picker';
  import {  faDisplay, faSearch, faSquarePlus, faTrash, faFont } from "@fortawesome/free-solid-svg-icons";
  let { itemReturn, currMenuItem = $bindable() }: {itemReturn: any, currMenuItem: zapScriptCmd } = $props();
  let currSelMenu: zapScriptCmd = $state(UIDUtils.getNewZapScriptCmd());
  let srchDialog: HTMLDialogElement;
  let srchItemDialog: HTMLDialogElement;
  let showColPicker: HTMLDialogElement;
  let currSrchResult:string = $state("");
  let colPickType: string = $state("");
  let currMenuItemIdx: number = 0;
  let srchType: string = $state("");
  let rgb: any = $state({
    "r": 255,
    "g": 255,
    "b": 255,
    "a": 1
  });

  function saveItem(){
    itemReturn?.({});
  }

  function addNewMenuCmd(){
    let tmpAction = UIDUtils.getNewAction();
    currMenuItem.args.client[0].args.input.buttons[0].args.actions.push(tmpAction);
  }

  function delMenuCmd(delItemIdx: number){
    currMenuItem.args.client[0].args.input.buttons[0].args.actions.splice(delItemIdx, 1);
  }
    
  function showSrchDialog(){
    srchType = "";
    srchDialog.showModal();
  }

  function showSrchItemDialog(menuItmIndx: number){
    srchType = "item";
    currMenuItemIdx = menuItmIndx;
    srchItemDialog.showModal();
  }

  function showCP(colObject: any, colType: string){
    colPickType = colType;
    rgb.r = colObject.r;
    rgb.g = colObject.g;
    rgb.b = colObject.b;
    showColPicker.showModal();
  }

  function colButtSetText(ItmRGB: any){
    return `background-color: rgb(${ItmRGB.r}, ${ItmRGB.g}, ${ItmRGB.b})`;
  }

  function colButtSetScreen(ItmRGB: any){
    return `background-color: rgb(${ItmRGB.r}, ${ItmRGB.g}, ${ItmRGB.b})`;
  }

  function searchReturn(srchResult: any): void{
    if(srchType == "item"){
      currSrchResult = srchResult.selectedGame;
      srchItemDialog.close("true")
      currMenuItem.args.client[0].args.input.buttons[0].args.actions[currMenuItemIdx].cmd = "evaluate"
      currMenuItem.args.client[0].args.input.buttons[0].args.actions[currMenuItemIdx].args.zapscript = currSrchResult;
      currSrchResult = "";
    }else{
      currSrchResult = srchResult.selectedGame;
      srchDialog.close("true")
      currMenuItem.args.zapscript = currSrchResult;
      currSrchResult = "";
    }
    srchType = "";    
  }

  function updateItemRGB(){
    showColPicker.close('true');
    if(colPickType == "text"){
        currMenuItem.args.client[0].args.display.textColour.r = rgb.r;
        currMenuItem.args.client[0].args.display.textColour.g = rgb.g;
        currMenuItem.args.client[0].args.display.textColour.b = rgb.b;
    }else if(colPickType == "screen"){
        currMenuItem.args.client[0].args.display.screenColour.r = rgb.r;
        currMenuItem.args.client[0].args.display.screenColour.g = rgb.g;
        currMenuItem.args.client[0].args.display.screenColour.b = rgb.b;
    }
    colPickType = "";
  }  

</script>
<div class="text-center mb-3">
  <h2>Menu Edit</h2>
</div>
<div class="text-center col-12">
  <div class="text-center d-flex flex-column flex-md-row align-items-center justify-content-between">
    <div class="text-center form-floating col-3">
      <input type="text" class="form-control" id="menuName" bind:value={currMenuItem.name}>
      <label for="menuName">Menu Item Name</label>
    </div>
    <div class="text-center form-floating col-8">
      <input type="text" class="form-control" id="itemZapScrpt" bind:value={currMenuItem.args.zapscript}/>
      <label for="itemZapScrpt">ZapScript Cmd</label>
    </div>
    <div class="text-center form-floating col-1">
      <button type="button" class="btn btn-primary" onclick={() => showSrchDialog()} data-bs-toggle="tooltip" title="Search For Game" data-bs-placement="top">
          <FontAwesomeIcon icon={faSearch}/>
      </button>
    </div>
  </div>
</div>
<div class="text-center col-12">
  <div class="text-center d-flex flex-column flex-md-row align-items-center justify-content-between">
    {#if currMenuItem?.args?.client[0]?.args?.display.displayText.length < 1}
    <div class="text-center form-floating col-6">
      <input type="text" class="form-control" id="itemImgPth" placeholder="/" bind:value={currMenuItem.args.client[0].args.display.imgPath}/>
      <label for="itemImgPth">JPEG Path</label>
    </div>
    {/if}
    {#if currMenuItem?.args?.client[0]?.args?.display.imgPath.length < 1}
    <div class="text-center form-floating col-8">
      <input type="text" class="form-control" id="itemText" bind:value={currMenuItem.args.client[0].args.display.displayText}/>
      <label for="itemText">Display Text</label>
    </div>
    <div class="text-center form-floating col-1">
        <button class="btn" onclick={() => showCP(currMenuItem.args.client[0].args.display.textColour, "text")} style={colButtSetText(currMenuItem.args.client[0].args.display.textColour)} data-bs-toggle="tooltip" title="Set Text Colour">
            <FontAwesomeIcon icon={faFont}/>
        </button>
        <button class="btn" onclick={() => showCP(currMenuItem.args.client[0].args.display.screenColour, "screen")} style={colButtSetScreen(currMenuItem.args.client[0].args.display.screenColour)} data-bs-toggle="tooltip" title="Set Screen Colour">
            <FontAwesomeIcon icon={faDisplay}/>
        </button>
    </div>
    <div class="text-center form-floating col-1">
      <input type="number" class="form-control" id="fntSize" bind:value={currMenuItem.args.client[0].args.display.fontSize} data-bs-toggle="tooltip" title="Suggest between 1-8"/>
      <label for="fntSize">Font Size</label>
    </div>
    <!-- <div class="text-center form-floating col-2">
      <input type="number" class="form-control" id="fntNumber" bind:value={currMenuItem.args.client[0].args.display.fontNumber} data-bs-toggle="tooltip" title="1-4"/>
      <label for="fntNumber">Font Num</label>
    </div> -->
    {/if}
  </div>
</div>
<div class="mt-4 pa-0">
  <h6>Menu Item Extra Cmds</h6>
</div>
<div class="col-12 menus-div container">
  <div class="table-responsive">
      <table class="table table-sm table-hover table-dark mt-0">
          <thead style="position: sticky;top: 0; padding: 1px !important;">
              <tr>
                  <th scope="col">
                      <button type="button" class="btn btn-primary mt-4" onclick={addNewMenuCmd} data-bs-toggle="tooltip" title="Add Menu Items" data-bs-placement="top">
                          <FontAwesomeIcon icon={faSquarePlus}/>
                      </button>
                  </th>
                  <th scope="col">ZapScript Cmd</th>
                  <th scope="col"></th>                  
              </tr>
          </thead>
          <tbody>
              {#each currMenuItem?.args?.client[0]?.args?.input?.buttons[0]?.args.actions as menuItem, i}
              {#if menuItem.args.uiPickerID.length < 1 && i > 0}
              <tr>
                  <td>
                      <button type="button" class="btn btn-primary" onclick={() => delMenuCmd(i)} data-bs-toggle="tooltip" title="Delete This Menu Item" data-bs-placement="top">
                          <FontAwesomeIcon icon={faTrash}/>
                      </button>
                  </td>
                  <td><input type="text" class="form-control" id="itemZapScrpt" bind:value={menuItem.args.zapscript}/></td>
                  <td>
                    <button type="button" class="btn btn-primary" onclick={() => showSrchItemDialog(i)} data-bs-toggle="tooltip" title="Search For Game" data-bs-placement="top">
                        <FontAwesomeIcon icon={faSearch}/>
                    </button>
                  </td>
              </tr>
              {/if}
              {/each}
          </tbody>
      </table>
  </div>
</div>
<div class="text-center mb-2 col-12">
    <button type="button" class="btn btn-primary mt-3" onclick={() => saveItem()}>Save & Exit</button>
</div>
{#key currSrchResult}
<dialog bind:this={srchDialog}>
  <div class="srchDialog">
      <SearchDialog {searchReturn}> </SearchDialog>
  </div>
</dialog>
{/key}

{#key currSrchResult}
<dialog bind:this={srchItemDialog}>
  <div class="srchDialog">
      <SearchDialog {searchReturn}> </SearchDialog>
  </div>
</dialog>
{/key}

<dialog bind:this={showColPicker}>
  <div class="colDialog dark">
      <ColorPicker bind:rgb position="responsive" isDialog={false} isDark={true} textInputModes={['rgb']}/>
  </div>
  <div class="align-items-center justify-content-between text-center mb-2 col-12">
      <button class="btn btn-primary" onclick={() => updateItemRGB()} >OK</button>
  </div>
</dialog>
