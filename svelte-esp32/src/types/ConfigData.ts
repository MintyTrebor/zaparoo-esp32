
export interface ConfigMessage{
  msgType: "ConfigData";
  data: ConfigData
}

export interface UIDExtdRecsMessage{
  msgType: "getUIDExtdRec";
  data: UIDExtdRecords;
}

export interface PushedUIDTokenMessage{
  msgType: "UIDTokenID";
  data: string;
}

export interface EspMessage{
  cmd: string;
  data: any;
}

export interface ConfigData {
  wifiLedEnabled: boolean;
  motorEnabled: boolean;
  launchLedEnabled: boolean;
  audioEnabled: boolean;
  pwrLedEnabled: boolean;
  resetOnRemove: boolean;
  zapIp: string;
  i2sDoutPin: number;
  i2sBclkPin: number;
  i2sLrcPin: number;
  audioGain: number;
  motorPin: number;
  wifiLedPin: number;
  launchLedPin: number;
  pwrLedPin: number;
  zapEnabled: boolean;
  steamEnabled: boolean;
  sdCardEnabled: boolean;
  steamIp: string;
  defaultLaunchAudio: string;
  defaultInsertAudio: string;
  defaultRemoveAudio: string;
  defaultErrorAudio: string;
  systemAudioEnabled: boolean;
  gameAudioEnabled: boolean;
  buzzOnDetect: boolean;
  buzzOnLaunch: boolean;
  buzzOnRemove: boolean;
  buzzOnError: boolean;
  PN532_module: boolean;
  zapWsPath: string;
  serialOnly: boolean;
  defaultImgPath: string;
};

export interface UIDExtdRecord {
  UID: string;
  launchAudio: string;
  removeAudio: string;
};

export interface UIDExtdRecords {
  UID_ExtdRecs: Array<UIDExtdRecord>;
}

export interface zapSystem {
  id: string;
  name: string;
  category: string;
};

export interface zapSystems {
  systems: Array<zapSystem>
}

export interface zapSearchResult {
  system: zapSystem;
  name: string;
  path: string;
}

export interface zapSearchResults {
  results: Array<zapSearchResult>;
  total: number;
}

export interface htmlFormattedSearchRec {
  path: string;
  name: string;
}

export interface htmlFormattedSearchRes {
  results: Array<htmlFormattedSearchRec>
}

export interface writeResultState {
  state: number
}

export interface indexingMessage {
  params: indexingParams
}

export interface indexingParams {
  totalSteps?: number,
  currentStep?: number,
  indexing: boolean,
  currentStepDisplay?: string,
  totalFiles?: number,
  exists?: boolean
}

export interface sourceZapSvsList {
  sources: Array<sourceZapSvs>
}

export interface sourceZapSvs {
  name: string,
  value: string
}

export interface mapping {
  added?: string;
  enabled?: boolean;
  id?: number;
  label?: string;
  match?:  string;
  override?:  string;
  pattern:  string;
  type?:  string;
}

export interface mappings {
  mappings: Array<mapping>;
}