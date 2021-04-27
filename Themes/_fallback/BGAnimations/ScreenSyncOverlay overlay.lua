local revert_sync_changes = THEME:GetString("ScreenSyncOverlay", "revert_sync_changes")
local change_bpm = THEME:GetString("ScreenSyncOverlay", "change_bpm")
local change_song_offset = THEME:GetString("ScreenSyncOverlay", "change_song_offset")
local change_machine_offset = THEME:GetString("ScreenSyncOverlay", "change_machine_offset")
local hold_alt = THEME:GetString("ScreenSyncOverlay", "hold_alt")

local sh_r = THEME:GetMetric("Common", "ScreenHeight")/480
local showadj = true

return Def.ActorFrame {
	Def.Quad {
		Name = "quad",
		InitCommand = function(self)
			self:diffuse {0, 0, 0, 0}:horizalign(right):vertalign(top):xy(_screen.w, 0)
		end,
		ShowCommand = function(self)
			self:stoptweening():linear(.3):diffusealpha(.5):sleep(6):linear(.3):diffusealpha(0)
		end,
		HideCommand = function(self)
			self:finishtweening()
		end
	},
	Def.BitmapText {
		Name = "help_text",
		Font = "Common Normal",
		InitCommand = function(self)
			local text = {
				revert_sync_changes .. ":",
				"    F4",
				change_song_offset .. ":",
				"    F11/F12",
				change_machine_offset .. ":",
				"    Shift + F11/F12",
				hold_alt
			}
			self:diffuse(1,1,1,0):horizalign(left):vertalign(top):shadowlength(2):settext(table.concat(text, "\n"))
               :zoom(math.min(1,0.7*sh_r)):xy( _screen.w - self:GetZoomedWidth() - 10, 10 )
			local quad = self:GetParent():GetChild("quad")
			quad:zoomtowidth(self:GetZoomedWidth() + 20):zoomtoheight(self:GetZoomedHeight() + 20)
		end,
		ShowCommand = function(self)
			self:visible(true)
			self:stoptweening():linear(.3):diffusealpha(1):sleep(6):linear(.3):diffusealpha(0)
		end,
		HideCommand = function(self)
			self:finishtweening()
			self:visible(false)
		end
	},
	Def.BitmapText {
		Name = "Status",
		Font = "ScreenSyncOverlay status",
		InitCommand = function(self)
			ActorUtil.LoadAllCommands(self, "ScreenSyncOverlay")
			self:playcommand("On")
		end,
		SetStatusCommand = function(self, param)
			self:settext(param.text)
            if GAMESTATE:GetPlayerState():GetCurrentPlayerOptions():UsingReverse() == true then
                self:y(SCREEN_CENTER_Y-160*sh_r)
            else
                self:y(SCREEN_CENTER_Y+160*sh_r)
            end
		end,
		HideCommand = function(self)
			self:settext("")
		end
	},
	Def.BitmapText {
		Name = "Adjustments",
		Font = "ScreenSyncOverlay adjustments",
		InitCommand = function(self)
			ActorUtil.LoadAllCommands(self, "ScreenSyncOverlay")
			self:playcommand("On"):xy(SCREEN_CENTER_X+(140*sh_r),SCREEN_CENTER_Y)
		end,
		SetAdjustmentsCommand = function(self, param)
			self:visible(param.visible):settext(param.text)
            if param.visible and showadj then
                if SCREEN_WIDTH/SCREEN_HEIGHT < 5.02/4 then
                    self:zoom(0.25*sh_r):bounceend(0.05):zoom(math.min(1,0.55*sh_r))
                elseif SCREEN_WIDTH/SCREEN_HEIGHT < 4.02/3 then
                    self:zoom(0.25*sh_r):bounceend(0.05):zoom(math.min(1,0.65*sh_r))
                else
                    self:zoom(0.25*sh_r):bounceend(0.05):zoom(math.min(1,0.85*sh_r))
                end
                showadj = false
            elseif not param.visible then
                showadj = true
            end
		end,
		HideCommand = function(self)
			self:settext("")
		end,
	},
}
