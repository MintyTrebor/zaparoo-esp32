<script lang="ts">
    import { UIDUtils } from "../backend/UIDUtils";
    import type { UIDExtdRecord, UIDFileJson } from "../types/ConfigData";
    import { CommonUtils } from "../backend/CommonUtils";
    import { onDestroy, onMount } from 'svelte';
    //let uidRecord: UIDExtdRecord = UIDUtils.getBlank();
    let uidFJson: UIDFileJson = UIDUtils.getBlankFileJson();
    let currUID: string;
    //UIDUtils.UIDRecord().subscribe(value=> uidRecord = value);
    UIDUtils.scannedUIDFileJson().subscribe(value=> updateScan(value));
    const handleSubmit = (event: Event) => {
        event.preventDefault();
        // const updRec = UIDUtils.getBlank();
        // updRec.UID = uidRecord.UID;
        // updRec.launchAudio = CommonUtils.validateAudioPath(uidRecord.launchAudio);
        // updRec.removeAudio = CommonUtils.validateAudioPath(uidRecord.removeAudio);
        // UIDUtils.updateUIDRecord(updRec);
        const updRec = UIDUtils.getBlankFileJson();
        updRec.launchAudio = CommonUtils.validateAudioPath(uidFJson.launchAudio);
        updRec.removeAudio = CommonUtils.validateAudioPath(uidFJson.removeAudio);
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
</script>
<div class="text-center mt-2">
    <h4>Scan an item to update audio controls</h4>
</div>
<form on:submit={handleSubmit} class="row g-1 mt-5">
    <div class="col-12">
        <div class="d-flex flex-column flex-md-row align-items-center justify-content-between">
            <div class="form-floating col-12 col-md-3">
                <input type="text" class="form-control" id="tokenUID" placeholder="/" bind:value={currUID}/>
                <label for="tokenUID">Token UID</label>
            </div>
            <div class="form-floating col-12 col-md-3">
                <input type="text" class="form-control" id="LaunPath" placeholder="/" bind:value={uidFJson.launchAudio}/>
                <label for="LaunPath">Launch Audio File</label>
            </div>
            <div class="form-floating col-12 col-md-3">
                <input type="text" class="form-control" id="RemPath" placeholder="/" bind:value={uidFJson.removeAudio}/>
                <label for="RemPath">Remove Audio File</label>
            </div>
        </div>
    </div>
    <button type="submit" class="btn btn-primary mt-3">Save</button>
</form>