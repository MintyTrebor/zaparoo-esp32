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
  import type {  zapScript, zapScriptCmd, pickerList } from "../types/ConfigData";
  import { UIDUtils } from "../backend/UIDUtils";
  import { onMount } from 'svelte';
  import { FontAwesomeIcon } from '@fortawesome/svelte-fontawesome';
  import SearchDialog from './ZapSearch.svelte';
  import ColorPicker from 'svelte-awesome-color-picker';
  import {  faEllipsis, faSearch, faSquarePlus, faTrash, faFont } from "@fortawesome/free-solid-svg-icons";
  import MenuItemEdit from './UIDMenuItemEdit.svelte';
  let { menuReturn, uidFJson = $bindable(), currSelMenuID }: {menuReturn: any, uidFJson: zapScript, currSelMenuID: string } = $props();
  let currSelMenu: zapScriptCmd = $state(UIDUtils.getNewZapScriptCmd());
  let srchDialog: HTMLDialogElement;
  let itemEditDialog: HTMLDialogElement;
  let showColPicker: HTMLDialogElement;
  let currSrchResult:string = $state("");
  let currMenuItemID: string = "";
  let isMainMenu: boolean = $state(false);
  let currMainZapMenuID: string = $state("");
  let currPickerList: pickerList = $state(UIDUtils.getBlankPickerList());
  let currMenuItem: zapScriptCmd = $state(UIDUtils.getNewZapScriptCmd());
  let rgb: any = $state({
    "r": 255,
    "g": 255,
    "b": 255,
    "a": 1
  });

  onMount(()=>{
    setCurrSelMenu();
  });

  function saveMenu(){
    menuReturn?.({});
  }

  function addNewMenuItem(){
    let tmpItem: zapScriptCmd = UIDUtils.getNewMenuItem();
    tmpItem.cmd = "evaluate";
    console.log("currSelMenu", currSelMenu)
    console.log("tmpItem", tmpItem)
    if(Array.isArray(currSelMenu.args.items)){
      currSelMenu.args.items.push(tmpItem);
    }else{
      currSelMenu.args.items = [];
      currSelMenu.args.items.push(tmpItem);
    }
  }

  function delCurrSelMenuItemID(delItemID: string, delItemIdx: number){
    if(currSelMenu.args.items[delItemIdx].id == delItemID){
        currSelMenu.args.items.splice(delItemIdx, 1);
    }
  }
  
  function showSrchDialog(menuItmId: string){
    currMenuItemID = menuItmId;
    srchDialog.showModal();
  }

  function showCP(colObject: any, menuItmId: string){
    currMenuItemID = menuItmId;
    showColPicker.showModal();
  }

  function showMenuItemEdit(menuItmId: string){
    currMenuItemID = menuItmId;
    let tmpArr = currSelMenu.args.items.filter((item: {id: string}) => (item.id == currMenuItemID));
    if(tmpArr.length > 0){
      currMenuItem = tmpArr[0];
    }
    itemEditDialog.showModal();
  }

  function colButtSet(ItmRGB: any){
    return `background-color: rgb(${ItmRGB.r}, ${ItmRGB.g}, ${ItmRGB.b})`;
  }

  function searchReturn(srchResult: any): void{
    currSrchResult = srchResult.selectedGame;
    srchDialog.close("true")
    let tmpArr = currSelMenu.args.items.filter((item: {id: string}) => (item.id == currMenuItemID));
    if(tmpArr.length > 0){
      tmpArr[0].args.zapscript  = currSrchResult;
    }
    currSrchResult = "";
  }

  function updateItemRGB(){
    showColPicker.close('true');
    let tmpArr = currSelMenu.args.items.filter((item: {id: string}) => (item.id == currMenuItemID));
    if(tmpArr.length > 0){
      tmpArr[0].args.client[0].args.display.textColour.r = rgb.r;
      tmpArr[0].args.client[0].args.display.textColour.g = rgb.g;
      tmpArr[0].args.client[0].args.display.textColour.b = rgb.b;
    }
  }

  function itemReturn(tmpStr:string): void{
      itemEditDialog.close("true");
  }

  function setCurrSelMenu(){
    if(currSelMenuID.length > 0){
      let i: any = 0;
      let m: any = 0;      
      for(i in uidFJson.cmds){
        //first generate the list of existing zap (user created) menus
        if(uidFJson.cmds[i].cmd == "ui.picker"){
          currMainZapMenuID = uidFJson.cmds[i].id;
          currPickerList.pickers = [];
          for(m in uidFJson.cmds[i].args.pickers){
            let tmpPicker = UIDUtils.geBlankZapScriptCmd();
            tmpPicker.id = uidFJson.cmds[i].args.pickers[m].id;
            tmpPicker.name = uidFJson.cmds[i].args.pickers[m].name
            currPickerList.pickers.push(tmpPicker);
          }
        }
        m = 0;
        if(uidFJson.cmds[i].cmd == "ui.picker" && uidFJson.cmds[i].id == currSelMenuID){
            currSelMenu = uidFJson.cmds[i];
            isMainMenu = true;
            return
        }else{
            for(m in uidFJson.cmds[i].args.pickers){
              if(uidFJson.cmds[i].args.pickers[m].cmd == "ui.picker" && uidFJson.cmds[i].args.pickers[m].id == currSelMenuID){
                  currSelMenu = uidFJson.cmds[i].args.pickers[m];
                  isMainMenu = false;
                  return;
              }
            }
        }       
      }
    }
    currSelMenu = UIDUtils.getBlankZapScriptCmd(); 
    return
  }

</script>
<div class="text-center mb-3">
  <h2>Menu Edit</h2>
</div>
<div class="text-center col-12">
  <div class="text-center d-flex flex-column flex-md-row align-items-center justify-content-between">
    <div class="text-center form-floating col-6">
      <input type="text" class="form-control" id="menuName" bind:value={currSelMenu.name}>
      <label for="menuName">Menu Name</label>
    </div>
  </div>
</div>
<div class="mt-4 pa-0">
  <h6>Menu Items</h6>
</div>
<div class="col-12 menus-div container">
  <div class="table-responsive">
      <table class="table table-sm table-hover table-dark mt-0">
          <thead style="position: sticky;top: 0; padding: 1px !important;">
              <tr>
                  <th scope="col">
                      <button type="button" class="btn btn-primary mt-4" onclick={addNewMenuItem} data-bs-toggle="tooltip" title="Add Menu Items" data-bs-placement="top">
                          <FontAwesomeIcon icon={faSquarePlus}/>
                      </button>
                  </th>
                  <th scope="col">Menu Item Name</th>
                  <th scope="col">ZapScript Cmd</th>
                  <th scope="col"></th>
                  <th scope="col">Img Path</th>
                  <th scope="col">Display Text</th>
                  <th scope="col"></th>
                  <!-- <th scope="col"></th> -->
                  <th scope="col">Open Menu</th>
                  <th scope="col"></th>
              </tr>
          </thead>
          <tbody>
              {#each currSelMenu?.args?.items as menuItem, i}
              <tr>
                  <td>
                      <button type="button" class="btn btn-primary" onclick={() => delCurrSelMenuItemID(menuItem.id, i)} data-bs-toggle="tooltip" title="Delete This Menu Item" data-bs-placement="top">
                          <FontAwesomeIcon icon={faTrash}/>
                      </button>
                  </td>
                  <td><input type="text" class="form-control" id="itemMnuName" bind:value={menuItem.name}/></td>
                  <td>
                    <!-- {#if menuItem.args?.client[0]?.args.input?.buttons[0]?.args?.actions[0]?.args.uiPickerID.length < 1} -->
                    <input type="text" class="form-control" id="itemZapScrpt" bind:value={menuItem.args.zapscript}/>
                    <!-- {/if} -->
                  </td>
                  <td>
                    <!-- {#if menuItem.args?.client[0]?.args.input?.buttons[0]?.args?.actions[0]?.args.uiPickerID.length < 1} -->
                    <button type="button" class="btn btn-primary" onclick={() => showSrchDialog(menuItem.id)} data-bs-toggle="tooltip" title="Search For Game" data-bs-placement="top">
                        <FontAwesomeIcon icon={faSearch}/>
                    </button>
                    <!-- {/if} -->
                  </td>
                  <td>
                    {#if menuItem.args.client[0].args.display.displayText.length < 1}
                    <input type="text" class="form-control" id="itemImgPth" placeholder="/" bind:value={menuItem.args.client[0].args.display.imgPath}/>
                    {/if}
                  </td>
                  <td>
                    {#if menuItem.args.client[0].args.display.imgPath.length < 1}
                    <input type="text" class="form-control" id="itemText" bind:value={menuItem.args.client[0].args.display.displayText}/>
                    {/if}
                  </td>
                  <td>
                    {#if menuItem.args.client[0].args.display.imgPath.length < 1}
                      <button class="btn" onclick={() => showCP(menuItem.args.client[0].args.display.textColour, menuItem.id)} style={colButtSet(menuItem.args.client[0].args.display.textColour)} data-bs-toggle="tooltip" title="Set Text Colour">
                          <FontAwesomeIcon icon={faFont}/>
                      </button>
                    {/if}
                  </td>
                <td>
                  <!-- {#if menuItem.args.zapscript.length < 1} -->
                  <select class="form-select" id="itmActData" bind:value={menuItem.args.client[0].args.input.buttons[0].args.actions[0].args.uiPickerID} data-bs-toggle="tooltip" title="Select Menu to Open" data-bs-placement="top">
                    <option value=""></option>  
                    <option value="9999">Reader Main Menu</option>
                      {#if !isMainMenu}
                      <option value={currMainZapMenuID}>Main Zap Menu</option>
                      {/if}
                      {#each currPickerList.pickers as {id, name}}
                      <option value={id}>{name}</option>
                      {/each}
                  </select>
                  <!-- {/if} -->
                </td>
                <td>
                    <button type="button" class="btn btn-primary" onclick={() => showMenuItemEdit(menuItem.id)} data-bs-toggle="tooltip" title="Edit Item" data-bs-placement="top">
                        <FontAwesomeIcon icon={faEllipsis}/>
                    </button>
                </td>
              </tr>
              {/each}
          </tbody>
      </table>
  </div>
</div>
<div class="text-center mb-2 col-12">
    <button type="button" class="btn btn-primary mt-3" onclick={() => saveMenu()}>Save & Exit</button>
</div>
{#key currSrchResult}
<dialog bind:this={srchDialog}>
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

{#key currMenuItemID}
<dialog bind:this={itemEditDialog}>
    <div class="srchDialog">
        <MenuItemEdit {itemReturn} bind:currMenuItem> </MenuItemEdit>
    </div>
</dialog>
{/key}
