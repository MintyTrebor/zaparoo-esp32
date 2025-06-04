import { writable, type Readable, type Writable } from "svelte/store";
import type { UIDExtdRecords, UIDExtdRecord, UIDExtdRecsMessage, PushedUIDTokenMessage, EspMessage, UIDFileJson, menuItem, menu, zapScript, zapScriptCmd, client, audio, display, colour, input, button, action, zapScriptPickerItem, pickerList } from "../types/ConfigData";
import { EspUtils } from "./EspUtils";
import { LogUtils } from "./LogUtils";
import {v4 as uuidv4} from 'uuid';

export class UIDUtils{
    private static currentUIDData: UIDExtdRecords;
    private static currentUIDRecord: Writable<UIDExtdRecord> = writable({} as UIDExtdRecord);
    private static isUIDModeEnabled= false;
    private static currentScannedFileJson: Writable<zapScript> = writable({} as zapScript);
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
        // if(!this.isUIDModeEnabled){
        //     let currData: zapScript = this.getNewUIDFileStructure();
        //     this.currentScannedFileJson.set(currData);
        //     this.currentScannedUID = "";
        // }
    }
    
    static getBlank(): UIDExtdRecord{
        return {} as UIDExtdRecord;
    }
    
    static UIDRecord(): Readable<UIDExtdRecord> {
        return this.currentUIDRecord;
    }

    static getBlankMenuItem(): menuItem{
        return {} as menuItem;
    }

    static getNewMenu(): menu{
        return {} as menu;
    }

    static getNewZapScript(): zapScript {
        let tmpObj: zapScript = {
            zapScript: 1,
            cmds:[]
        }
        return tmpObj;
    }

    static getNewAudio(): audio {
        let tmpObj: audio = {
            removeAudioPath: "",
            launchAudioPath: "",
            onClickAudioPath: ""
        }
        return tmpObj;
    }

    static getNewDisplay(): display {
        let tmpObj: display = {
            imgPath: "",
            displayText: "",
            textColour: this.getNewColour(),
            screenColour: this.getNewColour(),
            fontSize: 16,
            fontNumber: 2
        }
        return tmpObj;
    }

    static getNewZapScriptCmd(): zapScriptCmd {
        let tmpCmd: zapScriptCmd = this.buildNewZapScriptCmd();
        tmpCmd.id = uuidv4();
        return tmpCmd;
    }

    static geBlankZapScriptCmd(): zapScriptCmd {
        return {} as zapScriptCmd;
    }

    static getNewPickerItem(): zapScriptPickerItem {
        let tmpCmd: zapScriptPickerItem = this.buildNewPickerItem();
        tmpCmd.id = uuidv4();
        return tmpCmd;
    }

    private static buildNewZapScriptCmd(): zapScriptCmd {
        let tmpObj: zapScriptCmd = {
            id: uuidv4(),
            name: "",
            cmd: "",
            args: {
                zapscript: "",
                items: [],
                pickers:[],
                client: []
            }
        }
        return tmpObj;
    }

    private static buildNewPickerItem(): zapScriptPickerItem {
        let tmpObj: zapScriptPickerItem = {
            id: uuidv4(),
            name: "",
            cmd: "",
            args: {
                zapscript: "",
                client: []
            }
        }
        return tmpObj;
    }

    static getBlankZapScriptCmd(): zapScriptCmd {
        return {} as zapScriptCmd;
    }

    static getBlankPickerList(): pickerList {
        return {} as pickerList;
    }

    static getNewClient(): client {
        let tmpObj: client = {
            type: "",
            args: {
                audio: this.getNewAudio(),
                display: this.getNewDisplay(),
                input: this.getNewInput()
            }
        }
        return tmpObj;
    }

    static getNewColour(): colour {
        let tmpObj: colour = {
            r: 0,
            g: 0,
            b: 0
        }
        return tmpObj;
    }

    static getNewInput(): input {
        let tmpObj: input = {
            buttons: []
        }
        return tmpObj;
    }

    static getNewButton(): button {
        let tmpObj: button = {
            buttonID: "",
            args: {
                actions: []
            }
        }
        return tmpObj;
    }

    static getNewAction(): action {
        let tmpObj: action = {
            cmd: "",
            args:{
              uiPickerID: "",
              zapscript: "",
              audio: this.getNewAudio()
            }
        }
        return tmpObj;
    }

    static getNewUIDFileStructure(): zapScript {
        let tmpUIDFile: zapScript = this.getNewZapScript();
        let tmpClient: client = this.getNewClient();
        let tmpBtn: button = this.getNewButton();
        let tmpAction: action = this.getNewAction();
        tmpBtn.args.actions.push(tmpAction);
        tmpBtn.buttonID = "rotary";
        tmpClient.type = "reader";
        tmpClient.args.input.buttons.push(tmpBtn);
        tmpUIDFile.cmds.push(this.getNewZapScriptCmd());
        tmpUIDFile.cmds[0].args.client?.push(tmpClient);
        return tmpUIDFile;
    }

    static getNewMenuItem(): zapScriptCmd {
        let tmpUIDFile: zapScriptCmd = this.getNewZapScriptCmd();
        let tmpClient: client = this.getNewClient();
        let tmpBtn: button = this.getNewButton();
        let tmpAction: action = this.getNewAction();
        tmpBtn.args.actions.push(tmpAction);
        tmpBtn.buttonID = "rotary";
        tmpClient.type = "reader";
        tmpClient.args.input.buttons.push(tmpBtn);
        tmpUIDFile.args.client.push(tmpClient);
        return tmpUIDFile;
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

    
    static processUIDFileJson(fileData: zapScript, currUID: string){
        this.currentScannedUID = currUID;
        let currData: zapScript = fileData;
        this.currentScannedFileJson.set(currData);
    }

    static scannedUIDFileJson(): Readable<zapScript> {
        return this.currentScannedFileJson;
    }

    static saveUIDFileJson(fileData: zapScript){
        let newCMD = {
            UIDstr: this.currentScannedUID,
            fileJson: fileData
        }
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "/saveUIDFile", true);
        xhr.setRequestHeader('Content-Type', 'application/json');
        xhr.onreadystatechange = function() {
            if (xhr.readyState == 4 && xhr.status == 200) {
            }
        };
        xhr.send(JSON.stringify(newCMD));
    }

    private static saveSimpleUIDFileJson(fileData: zapScript, currUID: string){
        let newCMD = {
            UIDstr: currUID,
            fileJson: fileData
        }
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "/saveUIDFile", true);
        xhr.setRequestHeader('Content-Type', 'application/json');
        xhr.onreadystatechange = function() {
            if (xhr.readyState == 4 && xhr.status == 200) {
            }
        };
        xhr.send(JSON.stringify(newCMD));
    }

    static partialUpdUIDFileJson(currUID: string, aLaunchP: string | null, aRemoveP: string | null, launchImgP: string | null, zScript: string){
        if(!aLaunchP){aLaunchP = ""};
        if(!aRemoveP){aRemoveP = ""};
        if(!launchImgP){launchImgP = ""};
        let tmpZSJ: zapScript = this.getNewUIDFileStructure();
        tmpZSJ.cmds[0].cmd = "evaluate";
        tmpZSJ.cmds[0].args.zapscript = zScript;
        tmpZSJ.cmds[0].args.client[0].type = "reader";
        tmpZSJ.cmds[0].args.client[0].args.audio.launchAudioPath = aLaunchP;
        tmpZSJ.cmds[0].args.client[0].args.audio.removeAudioPath = aRemoveP;
        tmpZSJ.cmds[0].args.client[0].args.display.imgPath = launchImgP;
        this.saveSimpleUIDFileJson(tmpZSJ, currUID);
    }

    static getBlankFileJson(): UIDFileJson{
        return {} as UIDFileJson;
    }

    static currScannedUID(): string {
        return this.currentScannedUID;
    }
}