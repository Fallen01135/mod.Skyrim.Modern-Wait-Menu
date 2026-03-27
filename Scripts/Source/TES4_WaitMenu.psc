Scriptname TES4_WaitMenu extends Quest


;===================================================================================================
;IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII Properties IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
;===================================================================================================

; ============================== Editable ============================== ;

; ============================== Scripts =============================== ;

; ============================== Autofill ============================== ;


;===================================================================================================
;IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII Variables IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
;===================================================================================================

; ============================== Public =============================== ;

; ============================== Private ============================== ;


;===================================================================================================
;IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII Events IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
;===================================================================================================

event OnInit()
	RegisterForModEvent("TES4WaitMenu_UpdateWeather", "OnUpdateWeather")
	RegisterForModEvent("TES4WaitMenu_ShowCursor", "OnShowCurser")
endEvent

event OnUpdateWeather(string a_eventName, string a_strArg, float a_numArgk, Form a_sender)
	int weatherClassification = Weather.GetCurrentWeather().GetClassification()
	UI.InvokeInt("Sleep/Wait Menu", "_root.SleepWaitMenu_mc.updateWeather", weatherClassification)
endEvent

event OnShowCurser(string a_eventName, string a_strArg, float a_numArgk, Form a_sender)
	UI.SetBool("Cursor Menu", "_root.mc_Cursor._visible", a_numArgk as bool)
endEvent


;===================================================================================================
;IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII States IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
;===================================================================================================

;===================================================================================================
;IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII Functions IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
;===================================================================================================