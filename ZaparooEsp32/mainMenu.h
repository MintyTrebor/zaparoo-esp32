R"({
    "ID": "9999",
    "Name": "Main Menu",
    "cmd": "ui.picker",
    "args": {
        "zapscript": "",
        "items": [
            {
                "id": "9999-1",
                "name": "Now Playing",
                "cmd": "evaluate.client",
                "args": {
                    "zapscript": "",
                    "items": [],
                    "pickers": [],
                    "client": [
                        {
                            "type": "reader",
                            "args": {
                                "audio": {
                                    "launchAudioPath": "",
                                    "removeAudioPath": "",
                                    "onClickAudioPath": ""
                                },
                                "display": {
                                    "imgPath": "dispNowPlaying",
                                    "displayText": "",
                                    "textColour": {
                                        "r": 0,
                                        "g": 0,
                                        "b": 0
                                    },
                                    "screenColour": {
                                        "r": 255,
                                        "g": 255,
                                        "b": 255
                                    },
                                    "fontSize": 2,
                                    "fontNumber": 2
                                },
                                "input": {
                                    "buttons": [
                                        {
                                            "buttonID": "rotary",
                                            "args": {
                                                "actions": [
                                                    {
                                                        "cmd": "",
                                                        "args": {
                                                            "uiPickerID": "",
                                                            "cmd": "",
                                                            "audio": {
                                                                "removeAudioPath": "",
                                                                "launchAudioPath": "",
                                                                "onClickAudioPath": ""
                                                            }
                                                        }
                                                    }
                                                ]
                                            }
                                        }
                                    ]
                                }
                            }
                        }
                    ]
                }
            },
            {
                "id": "9999-2",
                "name": "Sleep",
                "cmd": "evaluate.client",
                "args": {
                    "zapscript": "",
                    "items": [],
                    "pickers": [],
                    "client": [
                        {
                            "type": "reader",
                            "args": {
                                "audio": {
                                    "launchAudioPath": "",
                                    "removeAudioPath": "",
                                    "onClickAudioPath": ""
                                },
                                "display": {
                                    "imgPath": "dispGotoSleep",
                                    "displayText": "",
                                    "textColour": {
                                        "r": 0,
                                        "g": 0,
                                        "b": 0
                                    },
                                    "screenColour": {
                                        "r": 255,
                                        "g": 255,
                                        "b": 255
                                    },
                                    "fontSize": 2,
                                    "fontNumber": 2
                                },
                                "input": {
                                    "buttons": [
                                        {
                                            "buttonID": "rotary",
                                            "args": {
                                                "actions": [
                                                    {
                                                        "cmd": "doDeepSleep",
                                                        "args": {
                                                            "uiPickerID": "",
                                                            "cmd": "",
                                                            "audio": {
                                                                "removeAudioPath": "",
                                                                "launchAudioPath": "",
                                                                "onClickAudioPath": ""
                                                            }
                                                        }
                                                    }
                                                ]
                                            }
                                        }
                                    ]
                                }
                            }
                        }
                    ]
                }
            },
            {
                "id": "9999-3",
                "name": "Power Off",
                "cmd": "evaluate.client",
                "args": {
                    "zapscript": "",
                    "items": [],
                    "pickers": [],
                    "client": [
                        {
                            "type": "reader",
                            "args": {
                                "audio": {
                                    "launchAudioPath": "",
                                    "removeAudioPath": "",
                                    "onClickAudioPath": ""
                                },
                                "display": {
                                    "imgPath": "dispPowerOff",
                                    "displayText": "",
                                    "textColour": {
                                        "r": 0,
                                        "g": 0,
                                        "b": 0
                                    },
                                    "screenColour": {
                                        "r": 255,
                                        "g": 255,
                                        "b": 255
                                    },
                                    "fontSize": 2,
                                    "fontNumber": 2
                                },
                                "input": {
                                    "buttons": [
                                        {
                                            "buttonID": "rotary",
                                            "args": {
                                                "actions": [
                                                    {
                                                        "cmd": "doShutdown",
                                                        "args": {
                                                            "uiPickerID": "",
                                                            "cmd": "",
                                                            "audio": {
                                                                "removeAudioPath": "",
                                                                "launchAudioPath": "",
                                                                "onClickAudioPath": ""
                                                            }
                                                        }
                                                    }
                                                ]
                                            }
                                        }
                                    ]
                                }
                            }
                        }
                    ]
                }
            }
        ]
    }
})"