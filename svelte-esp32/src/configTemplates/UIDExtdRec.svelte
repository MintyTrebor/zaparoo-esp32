<style>
    .menus-div{
        max-height: 25vh;
        min-height: 25vh;
    }
    .table-responsive {
        max-height: 25vh;
    }
    .srchDialog {
        min-height: 75vh;
        min-width: 75vw;
    }
</style>
<script lang="ts">
    import { UIDUtils } from "../backend/UIDUtils";
    import type { ConfigData, zapScript, zapScriptCmd, client } from "../types/ConfigData";
    import { CommonUtils } from "../backend/CommonUtils";
    import { onDestroy, onMount, tick } from 'svelte';
    import { EspUtils } from "../backend/EspUtils";
    import {v4 as uuidv4} from 'uuid';
    import { FontAwesomeIcon } from '@fortawesome/svelte-fontawesome';
    import { faEllipsis, faPaintBrush, faSearch, faSquarePlus, faTrash } from "@fortawesome/free-solid-svg-icons";
    import SearchDialog from './ZapSearch.svelte';
    import MenuEdit from './UIDMenuEdit.svelte';
    import ColorPicker from 'svelte-awesome-color-picker';
    let srchDialog: HTMLDialogElement;
    let menuEditDialog: HTMLDialogElement;
    let showColPicker: HTMLDialogElement;
    let config: ConfigData = $state(EspUtils.getBlank());
    let uidFJson: zapScript = $state(UIDUtils.getNewUIDFileStructure());
    let currUID: string = $state("");
    let currSelMenuID: string = $state("");
    let currSelMenu: zapScriptCmd = $state(UIDUtils.geBlankZapScriptCmd());
    let currSelMenuClientObject: client = $state(UIDUtils.getNewClient());
    let currMenuItemID: string = "";
    let currSrchResult:string = $state("");
    let currMainPickerID: string = $state("");
    let currMainPicker: zapScriptCmd = $state(UIDUtils.geBlankZapScriptCmd());
    let currMainPickerClientObject: client = $state(UIDUtils.getNewClient());
    let currFirstCmdID: string = $state("");
    let currFirstCmd: zapScriptCmd = $state(UIDUtils.geBlankZapScriptCmd());
    let currFirstCmdClientObject: client = $state(UIDUtils.getNewClient());
    UIDUtils.scannedUIDFileJson().subscribe(value=> updateScan(value));
    EspUtils.config().subscribe(value=> config = value);
    let rgb: any = $state({
        "r": 255,
        "g": 255,
        "b": 255,
        "a": 1
    });
    const handleSubmit = (event: Event) => {
        event.preventDefault();
        let i: any = 0;
        let k: any = 0;
        let l: any = 0;
        let j: any = 0;
        let m: any = 0;
        let n: any = 0;       
        for(i in uidFJson.cmds){
            let mainCmds: zapScriptCmd = uidFJson.cmds[i];
            if(mainCmds){
                for(k in mainCmds.args.client){
                    let currClient: client = mainCmds.args.client[k];
                    if(currClient){validateClient(currClient)};
                }
                for(j in mainCmds.args.items){
                    let currItem: zapScriptCmd = mainCmds.args.items[j];
                    if(currItem){
                        for(l in currItem.args.client){
                            let currClient: client = currItem.args.client[l];
                            if(currClient){validateClient(currClient)};
                        }
                    }
                }
                for(m in mainCmds.args.pickers){
                    let currPicker: zapScriptCmd = mainCmds.args.pickers[m];
                    if(currPicker){
                        for(n in currPicker.args.client){
                            let currClient: client = currPicker.args.client[n];
                            if(currClient){validateClient(currClient)};
                        }
                    }
                }
            }
        }  
        let updRec: zapScript = JSON.parse(JSON.stringify(uidFJson));
        console.log("savefilejson: ", updRec)
        UIDUtils.saveUIDFileJson(updRec);
    }

    function getMenulist(): any {
        let tmpPickers = uidFJson.cmds.filter((item: {cmd: string}) => (item.cmd == "ui.picker"));
        if(tmpPickers.length > 0){
            return tmpPickers;
        }else{
            return {};
        }
    }

    function validateClient(currClient: client){
        let l: any = 0;
        let j: any = 0;        
        if(currClient && currClient.type == "reader"){
            currClient.args.audio.launchAudioPath = CommonUtils.validateAudioPath(currClient.args.audio.launchAudioPath);
            currClient.args.audio.removeAudioPath = CommonUtils.validateAudioPath(currClient.args.audio.removeAudioPath);
            currClient.args.audio.onClickAudioPath = CommonUtils.validateAudioPath(currClient.args.audio.onClickAudioPath);
            currClient.args.display.imgPath = CommonUtils.validateAudioPath(currClient.args.display.imgPath);
            for(l in currClient.args.input.buttons){
                let currBtn = currClient.args.input.buttons[l];
                if(currBtn && currBtn.buttonID == "rotary"){
                    for(j in currBtn.args.actions){
                        let currAction = currBtn.args.actions[j];
                        if(currAction){
                            currAction.args.audio.onClickAudioPath = CommonUtils.validateAudioPath(currAction.args.audio.onClickAudioPath);
                            currAction.args.audio.launchAudioPath = CommonUtils.validateAudioPath(currAction.args.audio.launchAudioPath);
                            currAction.args.audio.removeAudioPath = CommonUtils.validateAudioPath(currAction.args.audio.removeAudioPath);
                        }
                    }
                }
            }
        }
    }

    onMount(()=>{
      UIDUtils.setUIDMode(true);
    });

    onDestroy(() => {
      UIDUtils.setUIDMode(false);
    });

    function updateScan(data: zapScript){
        uidFJson = data;
        console.log("Scanned DataFile Json: ", JSON.parse(JSON.stringify(uidFJson)))
        currUID = UIDUtils.currScannedUID();
        if('cmds' in uidFJson){
            currFirstCmdID = uidFJson.cmds[0].id;
            currFirstCmd = uidFJson.cmds[0];
            //find the 1st ui.picker as this is where all menu data goes
            let tmpPicker = uidFJson.cmds.filter((item: {cmd: string}) => (item.cmd == "ui.picker"));
            if(tmpPicker.length > 0){
                currMainPickerID = tmpPicker[0].id;
                currMainPicker = tmpPicker[0];
                if('client' in currMainPicker){
                    let tmpClient = currMainPicker.args.client.filter((item: {type: string}) => (item.type = "reader")); 
                    if(tmpClient.length > 0){
                        currMainPickerClientObject = tmpClient[0];
                    }else{
                        currMainPickerClientObject = UIDUtils.getNewClient();
                        currMainPickerClientObject.type = "reader";
                    }
                }else{
                    currMainPickerClientObject = UIDUtils.getNewClient();
                    currMainPickerClientObject.type = "reader";
                }
                //the first cmd rotary button action must always open the first picker so just set it to avoid errors
                let tmpInput = currMainPickerClientObject.args.input.buttons.filter((item: {buttonID: string}) => (item.buttonID == "rotary"));
                if(tmpInput.length > 0){
                    tmpInput[0].args.actions[0].cmd == "ui.picker";
                    tmpInput[0].args.actions[0].args.uiPickerID = currMainPickerID;
                }
            }else{
                currMainPickerID = "";
                currMainPicker = UIDUtils.geBlankZapScriptCmd();
            }
            if('client' in currFirstCmd.args){
                let tmpClient = currFirstCmd.args.client.filter((item: {type: string}) => (item.type = "reader"));
                if(tmpClient.length > 0){
                    currFirstCmdClientObject = tmpClient[0];
                }
                else{
                    let newCmd: client = UIDUtils.getNewClient();
                    newCmd.type = "reader";
                    currFirstCmd.args.client.push(newCmd);
                }
            }
        }

    }
    

    function setCurrSelMenuID(menuID: string){
        currSelMenuID = menuID;
        setCurrSelMenu();
        // currSelMenuClientObject = getClientObject(currSelMenu);
        tick().then(() =>{menuEditDialog.showModal()});
    } 

    function setCurrSelMenu(){
        if(currSelMenuID.length > 0){
            let i: any = 0;
            let m: any = 0;
            if(currMainPicker.id == currSelMenuID){
                //must be the main picker "Main Zap Menu"
                currSelMenu = currMainPicker;
            }else{
                //must be a sub picker
                for(i in uidFJson.cmds){
                    for(m in uidFJson.cmds[i].args.pickers){
                        if(uidFJson.cmds[i].args.pickers[m].cmd == "ui.picker" && uidFJson.cmds[i].args.pickers[m].id == currSelMenuID){
                            currSelMenu = uidFJson.cmds[i].args.pickers[m];
                            return;
                        }
                    }
                }
            }      
        }
        currSelMenu = UIDUtils.geBlankZapScriptCmd();
        return
    }

    function getClientObject(zpSC: zapScriptCmd){
        if("client" in zpSC.args){
            let tmpClient = currFirstCmd.args.client.filter((item: {type: string}) => (item.type = "reader"));
            if(tmpClient.length > 0){
                return tmpClient[0];
            } 
        }
        let tmpClient: client = UIDUtils.getNewClient();
        tmpClient.type = "reader";
        return tmpClient;
    }

    function addNewMenu(){
        let tmpMenu: zapScriptCmd = UIDUtils.getNewZapScriptCmd();
        tmpMenu.cmd = "ui.picker";
        //menus always go into main ui.picker as args.pickers
        if(currMainPicker.id){
            currMainPicker.args.pickers.push(tmpMenu);
            //currSelMenuID = tmpMenu.id;
            //currSelMenu = tmpMenu;
        }else {
            //this must be a new main menu
            tmpMenu.name = "Main Zap Menu"
            uidFJson.cmds.push(tmpMenu);
            let tmpPicker = uidFJson.cmds.filter((item: {cmd: string, id: string}) => (item.cmd == "ui.picker" && item.id == tmpMenu.id));
            currMainPicker = tmpPicker[0];

        }
    }

    function delCurrSelMenuID(delMenuID: string, delMenuIdx: number){
        if(currMainPicker.args.pickers[delMenuIdx].id == delMenuID){
            currMainPicker.args.pickers.splice(delMenuIdx, 1);
        }
    }

    function delCurrSelMenuItemID(delItemID: string, delItemIdx: number){
        if(currSelMenu.args.items[delItemIdx].id == delItemID){
            currSelMenu.args.items.splice(delItemIdx, 1);
        }
    }

    function addNewMenuItem(){
        let tmpItem: zapScriptCmd = UIDUtils.getNewZapScriptCmd();
        tmpItem.cmd = "ui.picker";
        tmpItem.args.client[0].type = "reader";
        tmpItem.args.client[0].args.input.buttons[0].buttonID = "rotary";
        currSelMenu.args.items.push(tmpItem);
    }

    

    function menuReturn(menuData: zapScriptCmd): void{
        menuEditDialog.close("true");
    }

    // function searchReturn(srchResult: any): void{
    //     currSrchResult = srchResult.selectedGame;
    //     srchDialog.close("true")
    //     let tmpArr = uidFJson.menus.filter((item: {menuID: string}) => (item.menuID == currSelMenuID));
    //         if(tmpArr.length > 0){
    //             let tmpMenu: menu = tmpArr[0];
    //             let tmpMenuItems = tmpMenu.menuItems.filter((item: {itemID: string}) => (item.itemID == currMenuItemID));
    //             if(tmpMenuItems.length > 0){
    //                 let tmpItem: menuItem = tmpMenuItems[0];
    //                 tmpItem.itemActionData = currSrchResult;
    //             }
    //         }
    // }
    
</script>
<div class="text-center">
    <h4>Scan a Token to update</h4>
</div>
<form onsubmit={handleSubmit} class="row g-1 mt-7">
    {#if currUID}
        <div class="col-12">
            <div class="d-flex flex-column flex-md-row align-items-center justify-content-between">
                <div class="form-floating col-3">
                    <input type="text" class="form-control" id="tokenUID" placeholder="/" bind:value={currUID}/>
                    <label for="tokenUID">Token UID</label>
                </div>
                <div class="form-floating col-3">
                    <input type="text" class="form-control" id="LaunPath" placeholder="/" bind:value={currFirstCmdClientObject.args.audio.launchAudioPath}/>
                    <label for="LaunPath">Launch Audio File</label>
                </div>
                <div class="form-floating col-3">
                    <input type="text" class="form-control" id="RemPath" placeholder="/" bind:value={currFirstCmdClientObject.args.audio.removeAudioPath}/>
                    <label for="RemPath">Remove Audio File</label>
                </div>
            </div>
            {#if config.deviceType == "Lilygo"}
            <div class="d-flex flex-column flex-md-row align-items-center justify-content-between">
                {#if currFirstCmdClientObject.args.display.displayText.length < 1}
                <div class="form-floating col-3">
                    <input type="text" class="form-control" id="aImgP" placeholder="/" bind:value={currFirstCmdClientObject.args.display.imgPath}/>
                    <label for="aImgP">JPEG Path</label>
                </div>
                {/if}
                {#if currFirstCmdClientObject.args.display.imgPath.length < 1}                
                <div class="form-floating col-3">
                    <input type="text" class="form-control" id="dispTxt" placeholder="/" bind:value={currFirstCmdClientObject.args.display.displayText}/>
                    <label for="dispTxt">Menu Text</label>
                </div>
                {/if}
                <!-- {#if currMainPicker.args.pickers.length > 0}
                <div class="form-floating col-3">
                    <select class="form-select" id="itmActData" bind:value={currFirstCmdClientObject.args.input.buttons.} data-bs-toggle="tooltip" title="Select Menu to Open" data-bs-placement="top">
                      <option value="9999">Main Menu</option>
                      {#each getMenulist as {id, name}}
                      <option value={id}>{name}</option>
                      {/each}
                    </select>
                </div> 
                {/if} -->
            </div>
            {/if}
        </div>
        <div class="form-floating col-2">
            <input type="text" class="form-control" id="menujson" value="{JSON.stringify(uidFJson)}">
            <label for="menuName">uidFJson JSON</label>
        </div>
        <div class="form-floating col-2">
            <input type="text" class="form-control" id="menujson" value="{JSON.stringify(currMainPicker)}">
            <label for="menuName">currMainPicker JSON</label>
        </div>
        <div class="form-floating col-2">
            <input type="text" class="form-control" id="menujson" value="{currSelMenuID}">
            <label for="menuName">currSelMenuID</label>
        </div>
        {#if config.deviceType == "Lilygo"}
            <div class="mt-3 mb-0 pb-0">
                <h6>Custom Menus</h6>
            </div>
            <div class="col-12 menus-div container ma-0 pa-0">
                <div class="table-responsive">
                    <table class="table table-sm table-hover table-dark mt-0">
                        <thead style="position: sticky;top: 0; padding: 1px !important;">
                            <tr>
                                <th scope="col" class="pt-0">
                                    <button type="button" class="btn btn-primary mt-4" onclick={addNewMenu} data-bs-toggle="tooltip" title="Add New Menu" data-bs-placement="top">
                                        <FontAwesomeIcon icon={faSquarePlus}/>
                                    </button>
                                </th>
                                <th scope="col" class="pt-0">Menu Name*</th>
                                <th scope="col" class="pt-0"></th>
                            </tr>
                        </thead>
                        <tbody>
                            {#if currMainPicker.id}
                            <tr>
                                <td>
                                    <button type="button" class="btn btn-primary" onclick={() => delCurrSelMenuID(currMainPicker.id, 0)} data-bs-toggle="tooltip" title="Delete This Menu" data-bs-placement="top">
                                        <FontAwesomeIcon icon={faTrash}/>
                                    </button>
                                </td>
                                <td>
                                    <input type="text" class="form-control" id="menuName" bind:value={currMainPicker.name} placeholder="Main Menu">
                                </td>
                                <td>
                                    <button type="button" class="btn btn-primary" onclick={() => setCurrSelMenuID(currMainPicker.id)} data-bs-toggle="tooltip" title="Edit Menu Items" data-bs-placement="top">
                                        <FontAwesomeIcon icon={faEllipsis}/>
                                    </button>
                                </td>
                            </tr>
                            {#each currMainPicker?.args?.pickers as menu, i}
                            <tr>
                                <td>
                                    <button type="button" class="btn btn-primary" onclick={() => delCurrSelMenuID(menu.id, i)} data-bs-toggle="tooltip" title="Delete This Menu" data-bs-placement="top">
                                        <FontAwesomeIcon icon={faTrash}/>
                                    </button>
                                </td>
                                <td>
                                    <input type="text" class="form-control" id="menuName" bind:value={menu.name}/>
                                </td>
                                <td>
                                    <button type="button" class="btn btn-primary" onclick={() => setCurrSelMenuID(menu.id)} data-bs-toggle="tooltip" title="Edit Menu Items" data-bs-placement="top">
                                        <FontAwesomeIcon icon={faEllipsis}/>
                                    </button>
                                </td>
                            </tr>
                            {/each}
                            {/if}
                        </tbody>
                    </table>
                </div>
            </div>
        {/if}
        <div class="text-center mb-2 col-12">
            <button type="submit" class="btn btn-primary mt-3">Save</button>
        </div>
    {/if}
</form>
{#key currSelMenuID}
<dialog bind:this={menuEditDialog}>
    <div class="srchDialog">
        <MenuEdit {menuReturn} bind:uidFJson {getMenulist} {currSelMenuID}> </MenuEdit>
    </div>
</dialog>
{/key}