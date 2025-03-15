import { writable, type Readable, type Writable } from "svelte/store";
import type { UIDExtdRecords, UIDExtdRecord, UIDExtdRecsMessage, PushedUIDTokenMessage, EspMessage, UIDFileJson, menuItem, menu } from "../types/ConfigData";
import { EspUtils } from "./EspUtils";
import { LogUtils } from "./LogUtils";

export class UIDUtils{
    private static currentUIDData: UIDExtdRecords;
    private static currentUIDRecord: Writable<UIDExtdRecord> = writable({} as UIDExtdRecord);
    private static isUIDModeEnabled= false;
    private static currentScannedFileJson: Writable<UIDFileJson> = writable({} as UIDFileJson);
    private static currentScannedUID: string;


    private static getBlankESPMsg(): EspMessage{
        return {} as EspMessage;
    }

    static setUIDMode(value: boolean){
        this.isUIDModeEnabled = value;
        let newCMD = this.getBlankESPMsg();
        console.log(`Setting UID Editing Mode : ${this.isUIDModeEnabled}`);
        newCMD.cmd = "set_UIDMode";
        newCMD.data = this.isUIDModeEnabled;
        if(!EspUtils.sendMessage(newCMD)){
            setTimeout(()=> this.setUIDMode(value), 2000);
        }
    }
    
    static getBlank(): UIDExtdRecord{
        return {} as UIDExtdRecord;
    }
    
    static UIDRecord(): Readable<UIDExtdRecord> {
        return this.currentUIDRecord;
    }

    static getNewMenuItem(): menuItem{
        return {} as menuItem;
    }

    static getNewMenu(): menu{
        return {} as menu;
    }

    static processUIDExtData(UIDData: UIDExtdRecsMessage){
        let currData: UIDExtdRecords = UIDData.data;
        this.currentUIDData = currData;
    }

    static processPushedUID(PushedUIDRecord: PushedUIDTokenMessage){
        let curRec = this.currentUIDData.UID_ExtdRecs.filter((item: {UID: string}) => (item.UID == PushedUIDRecord.data));
        if(curRec.length !== 0){
            this.currentUIDRecord.set(curRec[0]);
        }else{
            let tmpRec = this.getBlank();
            tmpRec.UID = PushedUIDRecord.data;
            tmpRec.launchAudio = "";
            tmpRec.removeAudio = "";
            this.currentUIDRecord.set(tmpRec);
        }
    }

    static updateUIDRecord(update: UIDExtdRecord){
        let bDidUpdate = false;
        let newCMD = this.getBlankESPMsg();
        for (let i = 0; i < this.currentUIDData.UID_ExtdRecs.length; i++) {
            if(this.currentUIDData.UID_ExtdRecs[i].UID == update.UID){
                this.currentUIDData.UID_ExtdRecs[i].launchAudio = update.launchAudio;
                this.currentUIDData.UID_ExtdRecs[i].removeAudio = update.removeAudio;
                bDidUpdate = true;
            }
        }
        if(!bDidUpdate){
            //new record
            const tmpRecord = this.getBlank();
            tmpRecord.UID = update.UID;
            tmpRecord.launchAudio = update.launchAudio;
            tmpRecord.removeAudio = update.removeAudio;
            this.currentUIDData.UID_ExtdRecs.push(tmpRecord)
        }
        newCMD.cmd = "saveUIDExtdRec";
        newCMD.data = this.currentUIDData;
        EspUtils.sendMessage(newCMD);
        LogUtils.notify("UID Control Record Saved");
    }

    
    static processUIDFileJson(fileData: UIDFileJson, currUID: string){
        this.currentScannedUID = currUID;
        let currData: UIDFileJson = fileData;
        this.currentScannedFileJson.set(currData);
    }

    static scannedUIDFileJson(): Readable<UIDFileJson> {
        return this.currentScannedFileJson;
    }

    static saveUIDFileJson(fileData: UIDFileJson){
        let newCMD = this.getBlankESPMsg();
        newCMD.cmd = "saveUIDFileJson";
        newCMD.data = {
            UIDstr: this.currentScannedUID,
            fileJson: fileData
        }
        EspUtils.sendMessage(newCMD);
        LogUtils.notify("UID Control File Saved");
    }

    static partialUpdUIDFileJson(currUID: string, aLaunchP: string | null, aRemoveP: string | null, launchImgP: string | null){
        let newCMD = this.getBlankESPMsg();
        newCMD.cmd = "partialUpdUidFileJson";
        newCMD.data = {
            UIDstr: currUID,
            fileJson: {
                launchAudio: aLaunchP,
                removeAudio: aRemoveP,
                launchImg: launchImgP,
                launchImgMenuID: null
            }
        }
        EspUtils.sendMessage(newCMD);
        LogUtils.notify("UID Control File Saved");
    }

    static getBlankFileJson(): UIDFileJson{
        return {} as UIDFileJson;
    }

    static currScannedUID(): string {
        return this.currentScannedUID;
    }
}