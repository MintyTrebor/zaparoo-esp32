import { writable, type Readable, type Writable } from "svelte/store";
import type { ConfigData, ConfigMessage, EspMessage } from "../types/ConfigData";
import { LogUtils } from "./LogUtils";
import { UIDUtils } from "./UIDUtils";
import { ZapUtils } from "./ZapUtils";

export class EspUtils{
    private static currentConfig: Writable<ConfigData> = writable({} as ConfigData);
    private static websocket: WebSocket;
    private static connected: Writable<boolean> = writable();
    private static intialLoad = false;
    private static updating = false;
    private static timerID: any = 0;
    
    static initWebSocket() {
        console.log('Trying to open a WebSocket connection to ZAP ESP…');
        this.websocket = new WebSocket('/ws');
        this.websocket.onopen    = this.onOpen.bind(this);
        this.websocket.onclose   = this.onClose.bind(this);
        this.websocket.onmessage = this.onMessage.bind(this);
    }

    static closeWebSocket(){
        this.websocket.send("{'cmd': 'closeWS'}");
        this.websocket.close();
    }

    private static keepAlive() { 
        let timeout = 10000;
        if (this.websocket.readyState == this.websocket.OPEN) {  
            this.websocket.send("{'cmd': 'ping'}");  
        }  
        this.timerID = setTimeout(this.keepAlive.bind(this), timeout);  
    }
    
    private static cancelKeepAlive() {  
        if (this.timerID) {  
            clearTimeout(this.timerID);  
        }  
    }

    private static onOpen() {
        LogUtils.addLogLine('Connection to ZAP ESP opened');
        console.log('Connection to ZAP ESP opened');
        this.websocket.send("{'cmd': 'get_Current_Config'}");
        this.keepAlive();			
    }
    
    private static onClose() {
        LogUtils.addLogLine('Connection to ZAP ESP closed');
        console.log('Connection to ZAP ESP closed');
        this.cancelKeepAlive();
    }
    
    private static onMessage(event: MessageEvent) {
        const msgData = JSON.parse(event.data);		
        switch(msgData.msgType){
            case "notify":
                if(msgData.data.type == "alert"){
                    LogUtils.notify(msgData.data.msgTxt);
                }
                if(msgData.data.type == "log"){
                    LogUtils.addLogLine(msgData.msgTxt);
                }
                break;
            case "ConfigData":
                this.currentConfig.set((msgData as ConfigMessage).data)
                LogUtils.notify(`Settings ${this.intialLoad ? "Updated" : "Loaded"}`);
                this.intialLoad = true;
                break;
            case "getUIDExtdRec":
                UIDUtils.processUIDExtData(msgData);
                break;
            case "UIDTokenID":
                UIDUtils.processPushedUID(msgData);
                break;
            case "writeResults":
                //console.log("wr: ", msgData.data)    
                ZapUtils.handleWriteResults(msgData.data.isSuccess, msgData.data.isCardDetected);
                break;
            case "closeWS":
                //console.log("wr: ", msgData)    
                this.websocket.onclose = null;
                this.websocket.close();
                setTimeout(()=> this.initWebSocket(), 2000);
                break;
        }
    }

    static async loadConfig(){}

    static config(): Readable<ConfigData>{
        return this.currentConfig;
    }

    static getBlank(): ConfigData{
        return {} as ConfigData;
    }

    static getConntected(): Readable<boolean>{
        return this.connected;
    }

    static updateConfig(update: Partial<ConfigData>){
        if(this.updating) return true;
        //console.log(update);
        this.updating = true;
        this.currentConfig.subscribe((conf=>{
            const data = Object.assign({...conf}, update);
            const payload = {
                cmd: "set_Current_Config",
                data: data
            };
            this.websocket.send(JSON.stringify(payload));
            LogUtils.notify("Settings updated, ESP will reload");
            this.updating = false;
            this.websocket.onclose = null;
            this.websocket.close();
            setTimeout(()=> this.initWebSocket(), 2000);
        }))();
    }

    static sendMessage(newMsg: EspMessage){
        try{
            this.websocket.send(JSON.stringify(newMsg));
            return true;
        }catch(e){
            return false;
        }
    }

    static updateWifi(ssid: string, password: string){
        const payload = {
            cmd: "wifi",
            data: {
                ssid: ssid,
                password: password
            }
        };
        this.websocket.send(JSON.stringify(payload));
        LogUtils.notify("Updated Wifi, ESP will now attempt to reconnect");
    }

    static resetESP32(){
        const payload = {
            cmd: "doReset"
        };
        this.websocket.send(JSON.stringify(payload));
        LogUtils.notify("Requested ESP Reset");
    }

    static async uploadFirmware(file: File, onProgress: (progress: number) => void) {
        const formData = new FormData();
        formData.append("update", file);
        const xhr = new XMLHttpRequest();
        xhr.open("POST", "/update", true);
        xhr.upload.onprogress = (event) => {
            if (event.lengthComputable) {
                const percentComplete = Math.round((event.loaded / event.total) * 100);
                onProgress(percentComplete);
            }
        };
        return new Promise<void>((resolve, reject) => {
            xhr.onload = () => {
                if (xhr.status >= 200 && xhr.status < 300) {
                    resolve();
                } else {
                    reject(new Error(`Upload failed with status: ${xhr.status}`));
                }
            };
            xhr.onerror = () => reject(new Error("Network error"));
            xhr.send(formData);
        });
    }
    
}