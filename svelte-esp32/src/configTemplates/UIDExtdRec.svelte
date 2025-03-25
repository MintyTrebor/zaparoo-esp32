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
    import { UIDUtils } from "../backend/UIDUtils";
    import type { UIDFileJson, ConfigData, menu, menuItem } from "../types/ConfigData";
    import { CommonUtils } from "../backend/CommonUtils";
    import { onDestroy, onMount } from 'svelte';
    import { EspUtils } from "../backend/EspUtils";
    import {v4 as uuidv4} from 'uuid';
    import { FontAwesomeIcon } from '@fortawesome/svelte-fontawesome';
    import { faEllipsis, faPaintBrush, faSearch, faSquarePlus, faTrash } from "@fortawesome/free-solid-svg-icons";
    import SearchDialog from './ZapSearch.svelte'
    import ColorPicker from 'svelte-awesome-color-picker';
    let srchDialog: HTMLDialogElement;
    let showColPicker: HTMLDialogElement;
    let config: ConfigData = $state(EspUtils.getBlank());
    let uidFJson: UIDFileJson = $state(UIDUtils.getBlankFileJson());
    let currUID: string = $state("");
    let currSelMenuID: string = $state("");
    let currSelMenu: menu = $state(UIDUtils.getNewMenu());
    let currMenuItemID: string = "";
    let currSrchResult:string = $state("");
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
        const updRec = UIDUtils.getBlankFileJson();
        updRec.launchAudio = CommonUtils.validateAudioPath(uidFJson.launchAudio);
        updRec.removeAudio = CommonUtils.validateAudioPath(uidFJson.removeAudio);
        updRec.launchImg = CommonUtils.validateAudioPath(uidFJson.launchImg);
        updRec.launchImgMenuID = uidFJson.launchImgMenuID;
        
        let i: any = 0;
        for(i in uidFJson.menus){
            uidFJson.menus[i].exitMenuImg = CommonUtils.validateAudioPath(uidFJson.menus[i].exitMenuImg);
            uidFJson.menus[i].exitMenuActionAudio = CommonUtils.validateAudioPath(uidFJson.menus[i].exitMenuActionAudio);
            let j: any =0;
            for(j in uidFJson.menus[i].menuItems){
                uidFJson.menus[i].menuItems[j].itemActionAudio = CommonUtils.validateAudioPath(uidFJson.menus[i].menuItems[j].itemActionAudio);
                uidFJson.menus[i].menuItems[j].itemImage = CommonUtils.validateAudioPath(uidFJson.menus[i].menuItems[j].itemImage);
                uidFJson.menus[i].menuItems[j].itemAudio = CommonUtils.validateAudioPath(uidFJson.menus[i].menuItems[j].itemAudio);
            }
            

        }        
        updRec.menus = uidFJson.menus;
        UIDUtils.saveUIDFileJson(updRec);
    }
    onMount(()=>{
      UIDUtils.setUIDMode(true);
    })
    onDestroy(() => {
      UIDUtils.setUIDMode(false);
    });
    function updateScan(data: UIDFileJson){
        uidFJson = data;
        currUID = UIDUtils.currScannedUID();
    }
    function setCurrSelMenuID(menuID: string){
        currSelMenuID = menuID;
        currSelMenu = getCurrSelMenu();
    } 
    function getCurrSelMenu(): menu {
        if(currSelMenuID.length > 0){
            let tmpArr = uidFJson.menus.filter((item: {menuID: string}) => (item.menuID == currSelMenuID));
            if(tmpArr.length > 0){
                return tmpArr[0];
            }else{
                return UIDUtils.getNewMenu();
            }
        }else{
            return UIDUtils.getNewMenu();
        }
    }

    function addNewMenu(){
        let tmpMenu: menu = UIDUtils.getNewMenu();
        tmpMenu.menuID = uuidv4();
        tmpMenu.exitMenuActionAudio = "";
        tmpMenu.exitMenuImg = "";
        tmpMenu.exitMenuText = "";
        tmpMenu.exitMenuTextColour.r = 0;
        tmpMenu.exitMenuTextColour.g = 0;
        tmpMenu.exitMenuTextColour.b = 0;
        tmpMenu.menuName = "";
        tmpMenu.menuItems = [];
        uidFJson.menus.push(tmpMenu);
    }
    function delCurrSelMenuID(delMenuID: string, delMenuIdx: number){
        if(uidFJson.menus[delMenuIdx].menuID == delMenuID){
            uidFJson.menus.splice(delMenuIdx, 1);
        }
    }
    function delCurrSelMenuItemID(delItemID: string, delItemIdx: number){
        if(currSelMenu.menuItems[delItemIdx].itemID == delItemID){
            currSelMenu.menuItems.splice(delItemIdx, 1);
        }
    }
    function addNewMenuItem(){
        let tmpItem: menuItem = UIDUtils.getNewMenuItem();
        tmpItem.itemID = uuidv4();
        tmpItem.itemActionAudio = "";
        tmpItem.itemActionData = "";
        tmpItem.itemActionType = "";
        tmpItem.itemAudio = "";
        tmpItem.itemImage = "";
        tmpItem.itemText = "";
        tmpItem.itemTextColour.r = 0;
        tmpItem.itemTextColour.g = 0;
        tmpItem.itemTextColour.b = 0;
        currSelMenu.menuItems.push(tmpItem);
    }

    function showSrchDialog(menuItmId: string){
        currMenuItemID = menuItmId;
        srchDialog.showModal();
    }

    function showCP(colObject: any, menuItmId: string){
        currMenuItemID = menuItmId;
        showColPicker.showModal();
    }

    function searchReturn(srchResult: any): void{
        currSrchResult = srchResult.selectedGame;
        srchDialog.close("true")
        let tmpArr = uidFJson.menus.filter((item: {menuID: string}) => (item.menuID == currSelMenuID));
            if(tmpArr.length > 0){
                let tmpMenu: menu = tmpArr[0];
                let tmpMenuItems = tmpMenu.menuItems.filter((item: {itemID: string}) => (item.itemID == currMenuItemID));
                if(tmpMenuItems.length > 0){
                    let tmpItem: menuItem = tmpMenuItems[0];
                    tmpItem.itemActionData = currSrchResult;
                }
            }
    }

    function updateItemRGB(){
        showColPicker.close('true');
        let tmpArr = uidFJson.menus.filter((item: {menuID: string}) => (item.menuID == currSelMenuID));
            if(tmpArr.length > 0){
                let tmpMenu: menu = tmpArr[0];
                let tmpMenuItems = tmpMenu.menuItems.filter((item: {itemID: string}) => (item.itemID == currMenuItemID));
                if(tmpMenuItems.length > 0){
                    let tmpItem: menuItem = tmpMenuItems[0];
                    tmpItem.itemTextColour.r = rgb.r;
                    tmpItem.itemTextColour.g = rgb.g;
                    tmpItem.itemTextColour.b = rgb.b;
                }
            }
    }

    function colButtSet(ItmRGB: any){
        return `background-color: rgb(${ItmRGB.r}, ${ItmRGB.g}, ${ItmRGB.b})`;
    }
</script>
<div class="text-center">
    <h4>Scan a Token to update</h4>
</div>
<form onsubmit={handleSubmit} class="row g-1 mt-7">
    {#if currUID}
        <div class="col-12">
            <div class="d-flex flex-column flex-md-row align-items-center justify-content-between">
                <div class="form-floating col-2">
                    <input type="text" class="form-control" id="tokenUID" placeholder="/" bind:value={currUID}/>
                    <label for="tokenUID">Token UID</label>
                </div>
                <div class="form-floating col-2">
                    <input type="text" class="form-control" id="LaunPath" placeholder="/" bind:value={uidFJson.launchAudio}/>
                    <label for="LaunPath">Launch Audio File</label>
                </div>
                <div class="form-floating col-2">
                    <input type="text" class="form-control" id="RemPath" placeholder="/" bind:value={uidFJson.removeAudio}/>
                    <label for="RemPath">Remove Audio File</label>
                </div>
                {#if config.deviceType == "Lilygo"}
                <div class="form-floating col-2">
                    <input type="text" class="form-control" id="aRemoveP" placeholder="/" bind:value={uidFJson.launchImg}/>
                    <label for="aRemoveP">JPEG Path</label>
                </div>
                    {#if uidFJson.menus.length > 0}
                    <div class="form-floating col-2">
                        <select class="form-select" id="selLaunchMenu" bind:value={uidFJson.launchImgMenuID}>
                            <option value="">No Action</option>
                            {#each uidFJson.menus as {menuID, menuName}}
                            <option value={menuID}>{menuName}</option>
                            {/each}
                        </select>
                        <label for="selLaunchMenu">Menu ID</label>
                    </div>
                    {/if}
                {/if}
            </div>
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
                                <th scope="col" class="pt-0">Exit Menu Image Path</th>
                                <th scope="col" class="pt-0">Exit Menu Text</th>
                                <th scope="col" class="pt-0">Exit Menu Audio Path</th>
                                <th scope="col" class="pt-0">Exit Menu Destination*</th>
                                <th scope="col" class="pt-0"></th>
                            </tr>
                        </thead>
                        <tbody>
                            {#each uidFJson.menus as menu, i}
                            <tr>
                                <td>
                                    <button type="button" class="btn btn-primary" onclick={() => delCurrSelMenuID(menu.menuID, i)} data-bs-toggle="tooltip" title="Delete This Menu" data-bs-placement="top">
                                        <FontAwesomeIcon icon={faTrash}/>
                                    </button>
                                </td>
                                <td>
                                    <input type="text" class="form-control" id="menuName" bind:value={menu.menuName}/>
                                </td>
                                <td>
                                    <input type="text" class="form-control" id="menuImg" placeholder="/" bind:value={menu.exitMenuImg}/>
                                </td>
                                <td>
                                    <input type="text" class="form-control" id="menuTxt" bind:value={menu.exitMenuText}/>
                                </td>
                                <td>
                                    <input type="text" class="form-control" id="menuActAudio" placeholder="/" bind:value={menu.exitMenuActionAudio}/>
                                </td>
                                <td>
                                    <select class="form-select" id="selExitMenu" bind:value={menu.exitMenuID}>
                                        <option value="9999">Main Menu</option>
                                        {#each uidFJson.menus as menuList}
                                        <option value={menuList.menuID}>{menuList.menuName}</option>
                                        {/each}
                                    </select>
                                </td>
                                <td>
                                    <button type="button" class="btn btn-primary" onclick={() => setCurrSelMenuID(menu.menuID)} data-bs-toggle="tooltip" title="Edit Menu Items" data-bs-placement="top">
                                        <FontAwesomeIcon icon={faEllipsis}/>
                                    </button>
                                </td>
                            </tr>
                            {/each}
                        </tbody>
                    </table>
                </div>
            </div>
            {#if currSelMenuID.length > 0}
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
                                <th scope="col">Menu Item Image Path</th>
                                <th scope="col">Menu Item Text</th>
                                <th scope="col"></th>
                                <th scope="col">On Click Audio Path</th>
                                <th scope="col">Item Action Type*</th>
                                <th scope="col">Item Action Data*</th>
                                <th scope="col">&nbsp;&nbsp;&nbsp;</th>
                            </tr>
                        </thead>
                        <tbody>
                            {#each currSelMenu.menuItems as menuItem, i}
                            <tr>
                                <td>
                                    <button type="button" class="btn btn-primary" onclick={() => delCurrSelMenuItemID(menuItem.itemID, i)} data-bs-toggle="tooltip" title="Delete This Menu Item" data-bs-placement="top">
                                        <FontAwesomeIcon icon={faTrash}/>
                                    </button>
                                </td>
                                <td><input type="text" class="form-control" id="itemImg" placeholder="/" bind:value={menuItem.itemImage}/></td>
                                <td><input type="text" class="form-control" id="itemTxt" bind:value={menuItem.itemText}/></td>
                                <td>
                                    <button class="btn" onclick={() => showCP(menuItem.itemTextColour, menuItem.itemID)} style={colButtSet(menuItem.itemTextColour)}>
                                        <FontAwesomeIcon icon={faPaintBrush}/>
                                    </button>
                                </td>
                                <td><input type="text" class="form-control" id="itemActAudio" placeholder="/" bind:value={menuItem.itemActionAudio}/></td>
                                <td>
                                    <select class="form-select" id="selItemAction" bind:value={menuItem.itemActionType}>
                                        <option value="menu">Goto Menu</option>
                                        <option value="launchGame">Launch Game</option>
                                        <option value="launchScript">Launch Script</option>
                                    </select>
                                </td>
                                {#if menuItem.itemActionType == "menu"}
                                <td>
                                    <select class="form-select" id="itmActData" bind:value={menuItem.itemActionData} data-bs-toggle="tooltip" title="Select Menu to Open" data-bs-placement="top">
                                        <option value="9999">Main Menu</option>
                                        {#each uidFJson.menus as {menuID, menuName}}
                                        <option value={menuID}>{menuName}</option>
                                        {/each}
                                    </select>
                                </td>
                                <td>&nbsp;&nbsp;&nbsp;</td>
                                {/if}
                                {#if menuItem.itemActionType == "launchGame"}
                                <td>
                                    <input type="text" class="form-control" id="itmActData" bind:value={menuItem.itemActionData} data-bs-toggle="tooltip" title="Enter Game launch Path" data-bs-placement="top"/>
                                </td>
                                <td>
                                    <button type="button" class="btn btn-primary" onclick={() => showSrchDialog(menuItem.itemID)} data-bs-toggle="tooltip" title="Search For Game" data-bs-placement="top">
                                        <FontAwesomeIcon icon={faSearch}/>
                                    </button>
                                </td>
                                {/if}
                                {#if menuItem.itemActionType == "launchScript"}
                                <td>
                                    <input type="text" class="form-control" id="itmActData" bind:value={menuItem.itemActionData} data-bs-toggle="tooltip" title="Enter Name of Script" data-bs-placement="top"/>
                                </td>
                                <td>&nbsp;&nbsp;&nbsp;</td>
                                {/if}
                                {#if menuItem.itemActionType == ""}
                                <td>
                                </td>
                                <td>&nbsp;&nbsp;&nbsp;</td>
                                {/if}
                            </tr>
                            {/each}
                        </tbody>
                    </table>
                </div>
            </div>
            {/if}
        {/if}
        <div class="text-center mb-2 col-12">
            <button type="submit" class="btn btn-primary mt-3">Save</button>
        </div>
    {/if}
</form>
<dialog bind:this={showColPicker}>
    <div class="colDialog dark">
        <ColorPicker bind:rgb position="responsive" isDialog={false} isDark={true} textInputModes={['rgb']}/>
    </div>
    <div class="align-items-center justify-content-between">
        <button class="btn btn-primary" onclick={() => updateItemRGB()} >OK</button>
    </div>
</dialog>
<dialog bind:this={srchDialog}>
    <div class="srchDialog">
        <SearchDialog {searchReturn}> </SearchDialog>
    </div>
</dialog>