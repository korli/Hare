//Name:		UserResizeSplitView.cpp
//Author:	Brian Tietz
//Copyright 1999
//Conventions:
//	Global constants (declared with const) and #defines - begin with "c_" followed by lowercase
//		words separated by underscores.
//		(E.G., #define c_my_constant 5).
//		(E.G., const int c_my_constant = 5;).
//	Global variables - begin with "g_" followed by lowercase words separated by underscores.
//		(E.G., int g_my_global;).
//	New data types (classes, structs, typedefs, etc.) - begin with an uppercase letter followed by
//		lowercase words separated by uppercase letters.  Enumerated constants contain a prefix
//		associating them with a particular enumerated set.
//		(E.G., typedef int MyTypedef;).
//		(E.G., enum MyEnumConst {c_mec_one, c_mec_two};)
//	Private member variables - begin with "m_" followed by lowercase words separated by underscores.
//		(E.G., int m_my_member;).
//	Public or friend-accessible member variables - all lowercase words separated by underscores.
//		(E.G., int public_member;).
//	Argument and local variables - begin with a lowercase letter followed by
//		lowercase words separated by underscores.  If the name is already taken by a public member
//		variable, prefix with a_ or l_
//		(E.G., int my_local; int a_my_arg, int l_my_local).
//	Functions (member or global) - begin with an uppercase letter followed by lowercase words
//		separated by uppercase letters.
//		(E.G., void MyFunction(void);).


//******************************************************************************************************
//**** System header files
//******************************************************************************************************
#include <Application.h>


//******************************************************************************************************
//**** Project header files
//******************************************************************************************************
#include "UserResizeSplitView.h"
#include "Cursors.h"
#include "Colors.h"


//******************************************************************************************************
//**** UserResizeSplitView
//******************************************************************************************************
UserResizeSplitView::UserResizeSplitView(BRect frame, const char* name, float divider_left_or_top,
	orientation posture, bool should_resize_left_or_top, bool should_resize_right_or_bottom,
	bool move_slider_on_frame_resize, uint32 resize_mask, uint32 flags)
: BView(frame,name,resize_mask,flags)
{
	m_divider_left_or_top = divider_left_or_top;
	m_posture = posture;
	m_should_resize_left_or_top = should_resize_left_or_top;
	m_should_resize_right_or_bottom = should_resize_right_or_bottom;
	m_move_slider_on_frame_resize = move_slider_on_frame_resize;
	m_left_or_top = NULL;
	m_right_or_bottom = NULL;
	m_left_or_top_minimum_size = 0;
	m_right_or_bottom_minimum_size = 0;
	m_modified_cursor = false;
	m_dragging = false;
	m_background_color = ui_color(B_PANEL_BACKGROUND_COLOR);
	m_dark_1_color = tint_color(m_background_color,B_DARKEN_1_TINT);
	m_dark_2_color = tint_color(m_background_color,B_DARKEN_2_TINT);
	SetViewColor(m_background_color);
	if(m_posture == B_HORIZONTAL)
		m_cached_width_or_height = frame.Height();
	else
		m_cached_width_or_height = frame.Width();
}


void UserResizeSplitView::AddChildren(BView* top_or_left, BView* right_or_bottom)
{
	m_left_or_top = top_or_left;
	m_right_or_bottom = right_or_bottom;
	AddChild(m_left_or_top);
	AddChild(m_right_or_bottom);
}


void UserResizeSplitView::SetMinimumChildAreaSizes(float left_or_top_minimum_size,
	float right_or_bottom_minimum_size)
{
	m_left_or_top_minimum_size = left_or_top_minimum_size;
	m_right_or_bottom_minimum_size = right_or_bottom_minimum_size;
}


void UserResizeSplitView::SetDividerLeftOrTop(float divider_left_or_top)
{
	BRect bounds = Bounds();
	if(divider_left_or_top < m_left_or_top_minimum_size+1)
		divider_left_or_top = m_left_or_top_minimum_size+1;
	if(m_posture == B_HORIZONTAL && divider_left_or_top > bounds.Height()-
		m_right_or_bottom_minimum_size-5)
	{
		divider_left_or_top = bounds.Height()-m_right_or_bottom_minimum_size-5;
	}
	else if(m_posture == B_VERTICAL && divider_left_or_top > bounds.Width()-
		m_right_or_bottom_minimum_size-5)
	{
		divider_left_or_top = bounds.Width()-m_right_or_bottom_minimum_size-5;
	}

	float delta = divider_left_or_top - m_divider_left_or_top;
	BRect invalid, frame;
	if(m_posture == B_HORIZONTAL)
	{
		if(m_should_resize_left_or_top && m_left_or_top)
			ResizeLeftOrTopChildTo(m_left_or_top->Frame().Height() + delta);
		if(m_right_or_bottom)
		{
			frame = m_right_or_bottom->Frame();
			m_right_or_bottom->MoveTo(frame.left,frame.top+delta);
			if(m_should_resize_right_or_bottom)
				ResizeRightOrBottomChildTo(frame.Height() - delta);
		}
		invalid.Set(bounds.left,MIN(m_divider_left_or_top,divider_left_or_top),bounds.right,
			MAX(m_divider_left_or_top,divider_left_or_top)+4);
	}
	else
	{
		if(m_should_resize_left_or_top && m_left_or_top)
			ResizeLeftOrTopChildTo(m_left_or_top->Frame().Width() + delta);
		if(m_right_or_bottom)
		{
			frame = m_right_or_bottom->Frame();
			m_right_or_bottom->MoveTo(frame.left+delta,frame.top);
			if(m_should_resize_right_or_bottom)
				ResizeRightOrBottomChildTo(m_right_or_bottom->Frame().Width() - delta);
		}
		invalid.Set(MIN(m_divider_left_or_top,divider_left_or_top),bounds.top,
			MAX(m_divider_left_or_top,divider_left_or_top)+4,bounds.bottom);
	}
	Invalidate(invalid);
	m_divider_left_or_top = divider_left_or_top;
}


void UserResizeSplitView::ResizeLeftOrTopChildTo(float width_or_height)
{
	if(m_posture == B_HORIZONTAL)
		m_left_or_top->ResizeTo(m_left_or_top->Frame().Width(),width_or_height);
	else
		m_left_or_top->ResizeTo(width_or_height,m_left_or_top->Frame().Height());
}


void UserResizeSplitView::ResizeRightOrBottomChildTo(float width_or_height)
{
	if(m_posture == B_HORIZONTAL)
		m_right_or_bottom->ResizeTo(m_left_or_top->Frame().Width(),width_or_height);
	else
		m_right_or_bottom->ResizeTo(width_or_height,m_left_or_top->Frame().Height());
}


void UserResizeSplitView::ResizeTo(float width, float height)
{
	if(m_posture == B_HORIZONTAL)
		m_cached_width_or_height = height;
	else
		m_cached_width_or_height = width;
	BView::ResizeTo(width,height);
}


void UserResizeSplitView::SetViewColor(rgb_color color)
{
	BView::SetViewColor(color);
	if(color.red != B_TRANSPARENT_COLOR.red || color.green != B_TRANSPARENT_COLOR.green ||
		color.blue != B_TRANSPARENT_COLOR.blue || color.alpha != B_TRANSPARENT_COLOR.alpha)
	{
		m_background_color = color;
		m_dark_1_color = tint_color(m_background_color,B_DARKEN_1_TINT);
		m_dark_2_color = tint_color(m_background_color,B_DARKEN_2_TINT);
	}
}


void UserResizeSplitView::FrameResized(float new_width, float new_height)
{
	float width_or_height = 0;
	if(m_posture == B_HORIZONTAL)
		width_or_height = new_height;
	else
		width_or_height = new_width;
	float delta = width_or_height - m_cached_width_or_height;
	BRect bounds = Bounds();
	if(m_move_slider_on_frame_resize)
	{
		BRect invalid;
		if(m_posture == B_HORIZONTAL)
			invalid.Set(bounds.left,MIN(m_divider_left_or_top,m_divider_left_or_top+delta),bounds.right,
				MAX(m_divider_left_or_top,m_divider_left_or_top+delta)+4);
		else
			invalid.Set(MIN(m_divider_left_or_top,m_divider_left_or_top+delta),bounds.top,
				MAX(m_divider_left_or_top,m_divider_left_or_top+delta)+4,bounds.bottom);
		Invalidate(invalid);
		m_divider_left_or_top += delta;
	}
	if(m_divider_left_or_top < m_left_or_top_minimum_size+1)
		SetDividerLeftOrTop(m_left_or_top_minimum_size+1);
	if(m_posture == B_HORIZONTAL && m_divider_left_or_top > bounds.Height()-
		m_right_or_bottom_minimum_size-5)
	{
		SetDividerLeftOrTop(bounds.Height()-m_right_or_bottom_minimum_size-5);
	}
	else if(m_posture == B_VERTICAL && m_divider_left_or_top > bounds.Width()-
		m_right_or_bottom_minimum_size-5)
	{
		SetDividerLeftOrTop(bounds.Width()-m_right_or_bottom_minimum_size-5);
	}
	m_cached_width_or_height = width_or_height;
}


void UserResizeSplitView::Draw(BRect updateRect)
{
	BRect bounds = Bounds();

	float visual_position = m_divider_left_or_top;
	SetHighColor(m_dark_2_color);
	if(m_posture == B_HORIZONTAL)
		StrokeLine(BPoint(bounds.left,visual_position),BPoint(bounds.right,visual_position));
	else
		StrokeLine(BPoint(visual_position,bounds.top),BPoint(visual_position,bounds.bottom));
	visual_position += 1;
	SetHighColor(White);
	if(m_posture == B_HORIZONTAL)
		StrokeLine(BPoint(bounds.left,visual_position),BPoint(bounds.right,visual_position));
	else
		StrokeLine(BPoint(visual_position,bounds.top),BPoint(visual_position,bounds.bottom));
	visual_position += 1;
	SetHighColor(m_background_color);
	if(m_posture == B_HORIZONTAL)
		StrokeLine(BPoint(bounds.left,visual_position),BPoint(bounds.right,visual_position));
	else
		StrokeLine(BPoint(visual_position,bounds.top),BPoint(visual_position,bounds.bottom));
	visual_position += 1;
	SetHighColor(m_dark_1_color);
	if(m_posture == B_HORIZONTAL)
		StrokeLine(BPoint(bounds.left,visual_position),BPoint(bounds.right,visual_position));
	else
		StrokeLine(BPoint(visual_position,bounds.top),BPoint(visual_position,bounds.bottom));
	visual_position += 1;
	SetHighColor(m_dark_2_color);
	if(m_posture == B_HORIZONTAL)
		StrokeLine(BPoint(bounds.left,visual_position),BPoint(bounds.right,visual_position));
	else
		StrokeLine(BPoint(visual_position,bounds.top),BPoint(visual_position,bounds.bottom));
}


void UserResizeSplitView::MouseDown(BPoint where)
{
	float mouse_position = 0;
	if(m_posture == B_HORIZONTAL)
		mouse_position = where.y;
	else
		mouse_position = where.x;
	if(mouse_position >= m_divider_left_or_top && mouse_position <= m_divider_left_or_top+4)
	{
		m_dragging = true;
		m_drag_mouse_offset = mouse_position-mouse_position;
		SetMouseEventMask(B_POINTER_EVENTS,B_NO_POINTER_HISTORY);
	}
}


void UserResizeSplitView::MouseUp(BPoint where)
{
	m_dragging = false;
	float mouse_position = 0;
	if(m_posture == B_HORIZONTAL)
		mouse_position = where.y;
	else
		mouse_position = where.x;
	if(!(mouse_position >= m_divider_left_or_top && mouse_position <= m_divider_left_or_top+4 &&
		Bounds().Contains(where)))
	{
		be_app->SetCursor(B_HAND_CURSOR);
		m_modified_cursor = false;
	}
}


void UserResizeSplitView::MouseMoved(BPoint where, uint32 code, const BMessage* message)
{
	bool should_show_modified_cursor = false;
	if(m_dragging)
		should_show_modified_cursor = true;
	else if(code != B_EXITED_VIEW)
	{
		float mouse_position = 0;
		if(m_posture == B_HORIZONTAL)
			mouse_position = where.y;
		else
			mouse_position = where.x;
		if(mouse_position >= m_divider_left_or_top && mouse_position <= m_divider_left_or_top+4)
			should_show_modified_cursor = true;
	}
	if(m_modified_cursor && !should_show_modified_cursor)
		be_app->SetCursor(B_HAND_CURSOR);
	if(should_show_modified_cursor && !m_modified_cursor)
		be_app->SetCursor((m_posture == B_HORIZONTAL)?c_h_resize_cursor:c_v_resize_cursor);
	m_modified_cursor = should_show_modified_cursor;
	if(m_dragging)
	{
		if(m_posture == B_HORIZONTAL)
			SetDividerLeftOrTop(where.y - m_drag_mouse_offset);
		else
			SetDividerLeftOrTop(where.x - m_drag_mouse_offset);
	}
}

