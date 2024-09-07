#if defined _INC_s_notification
	#endinput
#endif
#define _INC_s_notification

#include <a_samp>
#include <PawnPlus>
#include <YSI_Coding\y_timers>
#include <YSI_Coding\y_hooks>

#if !defined _INC_open_mp
	#if !defined PlayerTextDrawColour
		#define PlayerTextDrawColour PlayerTextDrawBoxColor
	#endif
#endif

#define MAX_SN_TITLE_STR (32)
#define MAX_SN_MESSAGE_STR (256)

static List:sn_List[MAX_PLAYERS];
static Timer:sn_Timer[MAX_PLAYERS];
static PlayerText:sn_TitleTD[MAX_PLAYERS];
static PlayerText:sn_MessageTD[MAX_PLAYERS];

hook OnPlayerConnect(playerid)
{
    if (list_valid(sn_List[playerid]))
    {
        list_delete_deep(sn_List[playerid]);
    }
    sn_List[playerid] = list_new();

    sn_TitleTD[playerid] = CreatePlayerTextDraw(playerid, 574.500, 116.000, "title");
    PlayerTextDrawLetterSize(playerid, sn_TitleTD[playerid], 0.230, 1.299);
    PlayerTextDrawTextSize(playerid, sn_TitleTD[playerid], 0.000, 109.000);
    PlayerTextDrawAlignment(playerid, sn_TitleTD[playerid], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, sn_TitleTD[playerid], -1);
    PlayerTextDrawUseBox(playerid, sn_TitleTD[playerid], true);
    PlayerTextDrawSetShadow(playerid, sn_TitleTD[playerid], 0);
    PlayerTextDrawSetOutline(playerid, sn_TitleTD[playerid], 0);
    PlayerTextDrawFont(playerid, sn_TitleTD[playerid], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, sn_TitleTD[playerid], true);

    sn_MessageTD[playerid] = CreatePlayerTextDraw(playerid, 520.000, 131.000, "message");
    PlayerTextDrawLetterSize(playerid, sn_MessageTD[playerid], 0.200, 1.199);
    PlayerTextDrawTextSize(playerid, sn_MessageTD[playerid], 629.000, 90.000);
    PlayerTextDrawAlignment(playerid, sn_MessageTD[playerid], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, sn_MessageTD[playerid], -1);
    PlayerTextDrawUseBox(playerid, sn_MessageTD[playerid], true);
    PlayerTextDrawSetShadow(playerid, sn_MessageTD[playerid], 0);
    PlayerTextDrawSetOutline(playerid, sn_MessageTD[playerid], 0);
    PlayerTextDrawFont(playerid, sn_MessageTD[playerid], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, sn_MessageTD[playerid], true);

    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    if (list_valid(sn_List[playerid]))
    {
        list_delete_deep(sn_List[playerid]);
    }
    return 1;
}

AddSNotification(
    playerid,
    _title[],
    _message[],
    display_time = 3000,
    soundid = 1083,
    title_bgcol = 877675007,
    title_col = -1,
    message_bgcol = 150,
    message_col = -1
)
{
    new title[MAX_SN_TITLE_STR], message[MAX_SN_MESSAGE_STR];
    format(title, sizeof title, "%s", _title);
    format(message, sizeof message, "%s", _message);
    strreplace(title, " ", "_");
    strreplace(message, " ", "_");

    if (list_size(sn_List[playerid]) > 0)
    {
        new String:current_data = list_get_str_safe_s(sn_List[playerid], 0);
        new List:current_list = str_split(current_data, " ");

        new current_title[MAX_SN_TITLE_STR];
		new current_message[MAX_SN_MESSAGE_STR];

        str_get(list_get_str_safe_s(current_list, 0), current_title);
        str_get(list_get_str_safe_s(current_list, 1), current_message);

        if (!strcmp(current_title, title) && !strcmp(current_message, message))
        {
            if (Timer_IsRunning(sn_Timer[playerid]))
            {
                stop sn_Timer[playerid];
            }
            list_set_str_s(sn_List[playerid], 0,
				str_format("%s %s %d %d %d %d %d %d",
					current_title, current_message,
					display_time, soundid,
					title_bgcol, title_col,
					message_bgcol, message_col
				)
			);
            sn_Timer[playerid] = defer CheckSNotification(playerid);
            return 1;
        }

        for (new i = 1, j = list_size(sn_List[playerid]); i < j; i++)
        {
            new String:data = list_get_str_safe_s(sn_List[playerid], i);
            new List:listt = str_split(data, " ");

            new existing_title[MAX_SN_TITLE_STR], existing_message[MAX_SN_MESSAGE_STR];
            str_get(list_get_str_safe_s(listt, 0), existing_title);
            str_get(list_get_str_safe_s(listt, 1), existing_message);

            if (!strcmp(existing_title, title) && !strcmp(existing_message, message))
            {
                list_set_str_s(sn_List[playerid], i,
					str_format("%s %s %d %d %d %d %d %d",
						existing_title, existing_message,
						display_time, soundid,
						title_bgcol, title_col,
						message_bgcol, message_col
					)
				);
                return 1;
            }
        }
    }
    list_add_str_s(sn_List[playerid],
		str_format("%s %s %d %d %d %d %d %d",
			title, message,
			display_time, soundid,
			title_bgcol, title_col,
			message_bgcol, message_col
		)
	);
    if (!Timer_IsRunning(sn_Timer[playerid]))
    {
        sn_Timer[playerid] = defer CheckSNotification(playerid);
    }

    return 1;
}

timer CheckSNotification[0](playerid)
{
    if (list_size(sn_List[playerid]) > 0)
    {
        new String:data = list_get_str_safe_s(sn_List[playerid], 0);
        new List:listt = str_split(data, " ");

        new title_str[MAX_SN_TITLE_STR], message_str[MAX_SN_MESSAGE_STR];
		new sound_str[32], display_time_str[32], mbgcol[32], mcol[32], tbgcol[32], tcol[32];
        str_get(list_get_str_safe_s(listt, 0), title_str);
        str_get(list_get_str_safe_s(listt, 1), message_str);
        str_get(list_get_str_safe_s(listt, 2), display_time_str);
        str_get(list_get_str_safe_s(listt, 3), sound_str);
		str_get(list_get_str_safe_s(listt, 4), tbgcol);
        str_get(list_get_str_safe_s(listt, 5), tcol);
		str_get(list_get_str_safe_s(listt, 6), mbgcol);
        str_get(list_get_str_safe_s(listt, 7), mcol);

        strreplace(title_str, "_", " ");
        strreplace(message_str, "_", " ");

        PlayerTextDrawSetString(playerid, sn_TitleTD[playerid], title_str);
        PlayerTextDrawSetString(playerid, sn_MessageTD[playerid], message_str);
		PlayerTextDrawBoxColour(playerid, sn_TitleTD[playerid], strval(tbgcol));
		PlayerTextDrawBackgroundColour(playerid, sn_TitleTD[playerid], strval(tcol));
		PlayerTextDrawBoxColour(playerid, sn_MessageTD[playerid], strval(mbgcol));
		PlayerTextDrawBackgroundColour(playerid, sn_MessageTD[playerid], strval(mcol));
        PlayerTextDrawShow(playerid, sn_MessageTD[playerid]);
        PlayerTextDrawShow(playerid, sn_TitleTD[playerid]);
        PlayerPlaySound(playerid, strval(sound_str));

        list_remove_deep(sn_List[playerid], 0);

        sn_Timer[playerid] = defer CheckSNotification[strval(display_time_str)](playerid);
    }
    else
    {
        PlayerTextDrawHide(playerid, sn_TitleTD[playerid]);
        PlayerTextDrawHide(playerid, sn_MessageTD[playerid]);
    }
    return 1;
}
