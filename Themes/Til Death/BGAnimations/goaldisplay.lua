local tzoom = 0.5
local pdh = 48 * tzoom
local ygap = 2
local packspaceY = pdh + ygap

local numgoals = 12
local ind = 0
local offx = 5
local width = SCREEN_WIDTH * 0.56
local dwidth = width - offx * 2
local height = (numgoals + 2) * packspaceY

local adjx = 10
local c0x = 20 -- for: priority and delete button
local c1x = c0x + 25 -- for: rate header, rate and percent
local c2x = c1x + (tzoom * 4 * adjx) -- for: song header and song name
local c5x = dwidth -- for: diff header, msd and steps diff
local c4x = c5x - adjx - (tzoom * 3.5 * adjx) -- for: date header, assigned, achieved
local c3x = c4x - adjx - (tzoom * 10 * adjx) -- for: filter header and song name
local headeroff = packspaceY / 1.5

local moving
local cheese

-- will eat any mousewheel inputs to scroll pages while mouse is over the background frame
local function input(event)
	if isOver(cheese:GetChild("FrameDisplay")) then
		if event.DeviceInput.button == "DeviceButton_mousewheel up" and event.type == "InputEventType_FirstPress" then
			moving = true
			cheese:queuecommand("PrevPage")
			return true
		elseif event.DeviceInput.button == "DeviceButton_mousewheel down" and event.type == "InputEventType_FirstPress" then
			cheese:queuecommand("NextPage")
			return true
		elseif moving == true then
			moving = false
		end
	end
	return false
end

local function highlight(self)
	if cheese:IsVisible() then
		self:queuecommand("Highlight")
	end
end

local function highlightIfOver(self)
	if isOver(self) then
		self:diffusealpha(0.6)
	else
		self:diffusealpha(1)
	end
end

local function byAchieved(scoregoal)
	if not scoregoal or scoregoal:IsAchieved() then
		return getMainColor("positive")
	end
	return color("#aaaaaa")
end
local filts = {
	THEME:GetString("TabGoals", "FilterAll"),
	THEME:GetString("TabGoals", "FilterCompleted"),
	THEME:GetString("TabGoals", "FilterIncomplete")
}

local translated_info = {
	PriorityLong = THEME:GetString("TabGoals", "PriorityLong"),
	PriorityShort = THEME:GetString("TabGoals", "PriorityShort"),
	RateLong = THEME:GetString("TabGoals", "RateLong"),
	RateShort = THEME:GetString("TabGoals", "RateShort"),
	Song = THEME:GetString("TabGoals", "Song"),
	Date = THEME:GetString("TabGoals", "Date"),
	Difficulty = THEME:GetString("TabGoals", "Difficulty"),
	Best = THEME:GetString("TabGoals", "Best"),
	Assigned = THEME:GetString("TabGoals", "AssignedDate"),
	Achieved = THEME:GetString("TabGoals", "AchievedDate"),
	Vacuous = THEME:GetString("TabGoals", "VacuousGoal"),
}

local goaltable
local o =
	Def.ActorFrame {
	Name = "GoalDisplay",
	InitCommand = function(self)
		cheese = self
		self:xy(0, 0)
		self:SetUpdateFunction(highlight)
	end,
	BeginCommand = function(self)
		SCREENMAN:GetTopScreen():AddInputCallback(input)
	end,
	OnCommand = function(self)
		GetPlayerOrMachineProfile(PLAYER_1):SetFromAll()
		self:queuecommand("GoalTableRefresh")
	end,
	GoalTableRefreshMessageCommand = function(self)
		goaltable = GetPlayerOrMachineProfile(PLAYER_1):GetGoalTable()
		ind = 0
		self:queuecommand("Update")
	end,
	UpdateCommand = function(self)
		if ind == #goaltable then
			ind = ind - numgoals
		elseif ind > #goaltable - (#goaltable % numgoals) then
			ind = #goaltable - (#goaltable % numgoals)
		end
		if ind < 0 then
			ind = 0
		end
	end,
	DFRFinishedMessageCommand = function(self)
		GetPlayerOrMachineProfile(PLAYER_1):SetFromAll()
		self:queuecommand("GoalTableRefresh")
	end,
	NextPageCommand = function(self)
		ind = ind + numgoals
		self:queuecommand("Update")
	end,
	PrevPageCommand = function(self)
		ind = ind - numgoals
		self:queuecommand("Update")
	end,
	Def.Quad {
		Name = "FrameDisplay",
		InitCommand = function(self)
			self:zoomto(width, height - headeroff):halign(0):valign(0):diffuse(color("#333333"))
		end
	},
	-- headers
	Def.Quad {
		InitCommand = function(self)
			self:xy(offx, headeroff):zoomto(dwidth, pdh):halign(0):diffuse(color("#111111"))
		end
	},
	LoadFont("Common normal") ..
		{
			--priority header
			InitCommand = function(self)
				self:xy(c0x, headeroff):zoom(tzoom):halign(0.5)
			end,
			UpdateCommand = function(self)
				self:settext(translated_info["PriorityShort"])
			end,
			HighlightCommand = function(self)
				if isOver(self) then
					self:settext(translated_info["PriorityLong"]):diffusealpha(0.6):halign(0.45):zoomx(0.7 * tzoom)
				else
					self:settext(translated_info["PriorityShort"]):diffusealpha(1):halign(0.5):zoomx(tzoom)
				end
			end,
			MouseLeftClickMessageCommand = function(self)
				if isOver(self) then
					GetPlayerOrMachineProfile(PLAYER_1):SortByPriority()
					ind = 0
					self:GetParent():queuecommand("GoalTableRefresh")
				end
			end
		},
	LoadFont("Common normal") ..
		{
			--rate header
			InitCommand = function(self)
				self:xy(c1x, headeroff):zoom(tzoom):halign(0.5)
			end,
			UpdateCommand = function(self)
				self:settext(translated_info["RateShort"])
			end,
			HighlightCommand = function(self)
				if isOver(self) then
					self:settext(translated_info["RateLong"]):diffusealpha(0.6)
				else
					self:settext(translated_info["RateShort"]):diffusealpha(1)
				end
			end,
			MouseLeftClickMessageCommand = function(self)
				if isOver(self) then
					GetPlayerOrMachineProfile(PLAYER_1):SortByRate()
					ind = 0
					self:GetParent():queuecommand("GoalTableRefresh")
				end
			end
		},
	LoadFont("Common normal") ..
		{
			--song header
			InitCommand = function(self)
				self:xy(c2x, headeroff):zoom(tzoom):halign(0):settext(translated_info["Song"])
			end,
			HighlightCommand = function(self)
				highlightIfOver(self)
			end,
			MouseLeftClickMessageCommand = function(self)
				if isOver(self) then
					GetPlayerOrMachineProfile(PLAYER_1):SortByName()
					ind = 0
					self:GetParent():queuecommand("GoalTableRefresh")
				end
			end
		},
	LoadFont("Common normal") ..
		{
			--index header
			InitCommand = function(self)
				self:xy(width / 2, headeroff):zoom(tzoom):halign(0.5)
			end,
			UpdateCommand = function(self)
				self:settextf("%i-%i", ind + 1, ind + numgoals)
			end
		},
	LoadFont("Common normal") ..
		{
			--completed toggle // filter header
			InitCommand = function(self)
				self:xy(width/2 + capWideScale(40, 50), headeroff):zoom(tzoom):halign(0):settext(filts[1])
			end,
			HighlightCommand = function(self)
				highlightIfOver(self)
			end,
			MouseLeftClickMessageCommand = function(self)
				if isOver(self) then
					GetPlayerOrMachineProfile(PLAYER_1):ToggleFilter()
					ind = 0
					self:settext(filts[GetPlayerOrMachineProfile(PLAYER_1):GetFilterMode()])
					self:GetParent():queuecommand("GoalTableRefresh")
				end
			end
		},
	LoadFont("Common normal") ..
		{
			--date header
			InitCommand = function(self)
				self:xy(c4x - capWideScale(5, 35), headeroff):zoom(tzoom):halign(1):settext(translated_info["Date"])
			end,
			HighlightCommand = function(self)
				highlightIfOver(self)
			end,
			MouseLeftClickMessageCommand = function(self)
				if isOver(self) then
					GetPlayerOrMachineProfile(PLAYER_1):SortByDate()
					ind = 0
					self:GetParent():queuecommand("GoalTableRefresh")
				end
			end
		},
	LoadFont("Common normal") ..
		{
			--diff header
			InitCommand = function(self)
				self:xy(c5x, headeroff):zoom(tzoom):halign(1):settext(translated_info["Difficulty"])
			end,
			HighlightCommand = function(self)
				highlightIfOver(self)
			end,
			MouseLeftClickMessageCommand = function(self)
				if isOver(self) then
					GetPlayerOrMachineProfile(PLAYER_1):SortByDiff()
					ind = 0
					self:GetParent():queuecommand("GoalTableRefresh")
				end
			end
		}
}

local function makeGoalDisplay(i)
	local sg
	local ck
	local goalsong
	local goalsteps

	local o =
		Def.ActorFrame {
		InitCommand = function(self)
			self:y(packspaceY * i + headeroff)
		end,
		UpdateCommand = function(self)
			sg = goaltable[(i + ind)]
			if sg then
				ck = sg:GetChartKey()
				goalsong = SONGMAN:GetSongByChartKey(ck)
				goalsteps = SONGMAN:GetStepsByChartKey(ck)
				self:queuecommand("Display")
				self:visible(true)
			else
				self:visible(false)
			end
		end,
		Def.Quad {
			InitCommand = function(self)
				self:x(offx):zoomto(dwidth, pdh):halign(0)
			end,
			DisplayCommand = function(self)
				self:diffuse(color("#111111CC"))
			end
		},
		LoadFont("Common normal") ..
			{
				--priority
				InitCommand = function(self)
					self:x(c0x):zoom(tzoom):halign(0.5):valign(1)
				end,
				DisplayCommand = function(self)
					self:settext(sg:GetPriority())
				end,
				HighlightCommand = function(self)
					highlightIfOver(self)
				end,
				MouseLeftClickMessageCommand = function(self)
					if isOver(self) and sg then
						sg:SetPriority(sg:GetPriority() + 1)
						self:GetParent():queuecommand("Update")
					end
				end,
				MouseRightClickMessageCommand = function(self)
					if isOver(self) and sg then
						sg:SetPriority(sg:GetPriority() - 1)
						self:GetParent():queuecommand("Update")
					end
				end
			},
		Def.Sprite {
			-- delete button
			Texture = THEME:GetPathG("","goalX.png"),
			InitCommand = function(self)
				self:xy(c0x - 13,pdh/2.3):zoom(0.3):halign(0):valign(1):diffuse(byJudgment("TapNoteScore_Miss"))
			end,
			HighlightCommand = function(self)
				highlightIfOver(self)
			end,
			MouseLeftClickMessageCommand = function(self)
				if isOver(self) then
					sg:Delete()
					GetPlayerOrMachineProfile(PLAYER_1):SetFromAll()
					self:GetParent():GetParent():queuecommand("GoalTableRefresh")
				end
			end
		},
		LoadFont("Common normal") ..
			{
				--rate
				InitCommand = function(self)
					self:x(c1x):zoom(tzoom):halign(0.5):valign(1)
				end,
				DisplayCommand = function(self)
					local ratestring = string.format("%.2f", sg:GetRate()):gsub("%.?0$", "") .. "x"
					self:settext(ratestring)
				end,
				HighlightCommand = function(self)
					highlightIfOver(self)
				end,
				MouseLeftClickMessageCommand = function(self)
					if isOver(self) and sg then
						sg:SetRate(sg:GetRate() + 0.1)
						self:GetParent():queuecommand("Update")
					end
				end,
				MouseRightClickMessageCommand = function(self)
					if isOver(self) and sg then
						sg:SetRate(sg:GetRate() - 0.1)
						self:GetParent():queuecommand("Update")
					end
				end
			},
		LoadFont("Common normal") ..
			{
				--percent
				InitCommand = function(self)
					self:x(c1x):zoom(tzoom):halign(0.5):valign(0):maxwidth((50 - capWideScale(10, 10)) / tzoom)
				end,
				DisplayCommand = function(self)
					local perc = notShit.round(sg:GetPercent() * 100000) / 1000
					if perc <= 99 or perc == 100 then
						self:settextf("%.f%%", perc)
					elseif (perc < 99.8) then
						self:settextf("%.2f%%", perc)
					else
						self:settextf("%.3f%%", perc)
					end
					self:diffuse(byAchieved(sg))
				end,
				HighlightCommand = function(self)
					highlightIfOver(self)
				end,
				MouseLeftClickMessageCommand = function(self)
					if isOver(self) and sg then
						sg:SetPercent(sg:GetPercent() + 0.01)
						self:GetParent():queuecommand("Update")
					end
				end,
				MouseRightClickMessageCommand = function(self)
					if isOver(self) and sg then
						sg:SetPercent(sg:GetPercent() - 0.01)
						self:GetParent():queuecommand("Update")
					end
				end
			},
		LoadFont("Common normal") ..
			{
				--song name
				InitCommand = function(self)
					self:x(c2x):zoom(tzoom):maxwidth((c3x - c2x - capWideScale(32, 62)) / tzoom):halign(0):valign(1)
				end,
				DisplayCommand = function(self)
					if goalsong then
						self:settext(goalsong:GetDisplayMainTitle()):diffuse(getMainColor("positive"))
					else
						self:settext(sg:GetChartKey()):diffuse(getMainColor("negative"))
					end
				end,
				HighlightCommand = function(self)
					highlightIfOver(self)
				end,
				MouseLeftClickMessageCommand = function(self)
					if sg then
						if isOver(self) and sg and goalsong and goalsteps then
							SCREENMAN:GetTopScreen():GetMusicWheel():SelectSong(goalsong)
							GAMESTATE:GetSongOptionsObject("ModsLevel_Preferred"):MusicRate(sg:GetRate())
							GAMESTATE:GetSongOptionsObject("ModsLevel_Song"):MusicRate(sg:GetRate())
							GAMESTATE:GetSongOptionsObject("ModsLevel_Current"):MusicRate(sg:GetRate())
							MESSAGEMAN:Broadcast("GoalSelected")
						end
					end
				end
			},
		LoadFont("Common normal") ..
			{
				--pb
				InitCommand = function(self)
					self:x(c2x):zoom(tzoom):halign(0):valign(0)
				end,
				DisplayCommand = function(self)
					local pb = sg:GetPBUpTo()
					if pb then
						if pb:GetMusicRate() < sg:GetRate() then
							local ratestring = string.format("%.2f", pb:GetMusicRate()):gsub("%.?0$", "") .. "x"
							self:settextf("%s: %5.2f%% (%s)", translated_info["Best"], pb:GetWifeScore() * 100, ratestring)
						else
							self:settextf("%s: %5.2f%%", translated_info["Best"], pb:GetWifeScore() * 100)
						end
						self:diffuse(getGradeColor(pb:GetWifeGrade()))
						self:visible(true)
					else
						self:settextf("(%s: %5.2f%%)", translated_info["Best"], 0)
						self:diffuse(byAchieved(sg))
					end
				end
			},
		LoadFont("Common normal") ..
			{
				--assigned
				InitCommand = function(self)
					self:x(c4x):zoom(tzoom):halign(1):valign(0):maxwidth(width / 4 / tzoom)
				end,
				DisplayCommand = function(self)
					self:settextf("%s: %s", translated_info["Assigned"], sg:WhenAssigned()):diffuse(byAchieved(sg))
				end
			},
		LoadFont("Common normal") ..
			{
				--achieved
				InitCommand = function(self)
					self:x(c4x):zoom(tzoom):halign(1):valign(1):maxwidth(width / 4 / tzoom)
				end,
				DisplayCommand = function(self)
					if sg:IsAchieved() then
						self:settextf("%s: %s", translated_info["Achieved"], sg:WhenAchieved())
					elseif sg:IsVacuous() then
						self:settext(translated_info["Vacuous"])
					else
						self:settext("")
					end
					self:diffuse(byAchieved(sg))
				end
			},
		LoadFont("Common normal") ..
			{
				--msd diff
				InitCommand = function(self)
					self:x(c5x):zoom(tzoom):halign(1):valign(1)
				end,
				DisplayCommand = function(self)
					if goalsteps then
						local msd = goalsteps:GetMSD(sg:GetRate(), 1)
						self:settextf("%5.1f", msd):diffuse(byMSD(msd))
					else
						self:settext("??")
					end
				end
			},
		LoadFont("Common normal") ..
			{
				--steps diff
				InitCommand = function(self)
					self:x(c5x):zoom(tzoom):halign(1):valign(0)
				end,
				DisplayCommand = function(self)
					if goalsteps and goalsong then
						local diff = goalsteps:GetDifficulty()
						self:settext(getShortDifficulty(diff))
						self:diffuse(byDifficulty(diff))
					else
						self:settext("??")
						self:diffuse(getMainColor("negative"))
					end
				end
			},
	}
	return o
end

for i = 1, numgoals do
	o[#o + 1] = makeGoalDisplay(i)
end

return o
