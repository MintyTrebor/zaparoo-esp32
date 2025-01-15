
export interface ConfigMessage{
  msgType: "ConfigData";
  data: ConfigData
}

export interface UIDExtdRecsMessage{
  msgType: "getUIDExtdRec";
  data: UIDExtdRecords;
}

export interface pushedUIDTokenMessage{
  msgType: "UIDTokenID";
  data: string;
}

export interface sendToESPMessage{
  cmd: string;
  data: any;
}

export interface ConfigData {
  wifi_led_enabled: boolean;
  motor_enabled: boolean;
  launch_led_enabled: boolean;
  audio_enabled: boolean;
  pwr_led_enabled: boolean;
  reset_on_remove_enabled: boolean;
  defAudioPath: string;
  ZapIP: string;
  pin_I2S_DOUT: number;
  pin_I2S_BCLK: number;
  pin_I2S_LRC: number;
  val_AUDIO_GAIN: number;
  pin_MOTOR_PIN: number;
  pin_WIFI_LED_PIN: number;
  pin_LAUNCH_LED_PIN: number;
  pin_EXTERNAL_POWER_LED: number;
  mister_enabled: boolean;
  steamOS_enabled: boolean;
  sdCard_enabled: boolean;
  SteamIP: string;
  defDetectAudioPath: string;
  defRemoveAudioPath: string;
  defErrAudioPath: string;
  systemAudio_enabled: boolean;
  gameAudio_enabled: boolean;
  buzz_on_detect_enabled: boolean;
  buzz_on_launch_enabled: boolean;
  buzz_on_remove_enabled: boolean;
  buzz_on_error_enabled: boolean;
  PN532_module: boolean;
  zap_ws_path: string;
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
  system: Array<zapSystem>;
  name: string;
  path: string;
}

export interface zapSearchResults {
  results: Array<zapSearchResult>;
  total: number;
}
