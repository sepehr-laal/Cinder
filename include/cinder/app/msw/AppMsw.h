/*
 Copyright (c) 2014, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "cinder/app/AppBase.h"
#include "cinder/msw/CinderWindowsFwd.h"
#include "cinder/app/RendererGl.h"

namespace cinder { namespace app {

class AppImplMswBasic;

class AppMsw : public AppBase {
  public:
	//! MSW-specific settings
	class Settings : public AppBase::Settings {
	  public:
		Settings() : mMswConsoleEnabled( false )				{}

		//! If enabled MSW apps will display a secondary window which captures all cout, cerr, cin and App::console() output. Default is \c false.
		void	setConsoleWindowEnabled( bool enable = true )	{ mMswConsoleEnabled = enable; }
		//! Returns whether MSW apps will display a secondary window which captures all cout, cerr, cin and App::console() output. Default is \c false.
		bool	isConsoleWindowEnabled() const					{ return mMswConsoleEnabled; }

	  private:
		void	pushBackCommandLineArg( const std::string &arg );

		bool	mMswConsoleEnabled;

		friend AppMsw;
	};

	typedef std::function<void( Settings *settings )>	SettingsFn;

	AppMsw();
	virtual ~AppMsw();

	WindowRef	createWindow( const Window::Format &format = Window::Format() ) override;
	void		quit() override;

	float		getFrameRate() const override;
	void		setFrameRate( float frameRate ) override;
	void		disableFrameRate() override;
	bool		isFrameRateEnabled() const override;

	WindowRef	getWindow() const override;
	WindowRef	getWindowIndex( size_t index ) const override;
	size_t		getNumWindows() const override;

	WindowRef	getForegroundWindow() const override;

	void		hideCursor() override;
	void		showCursor() override;
	ivec2		getMousePos() const override;

	//! \cond
	// Called from WinMain, forwards to AppBase::initialize() but also fills command line args using native windows API
	static void	initialize( Settings *settings, const RendererRef &defaultRenderer, const char *title );
	//! \endcond

  protected:
	void	launch() override;

  private:
	std::unique_ptr<AppImplMswBasic>	mImpl;
	bool								mConsoleWindowEnabled;

  public:
	  template<class App>
	  static void	prepareLaunch(
		  const std::string& title,
		  const RendererRef &renderer = RendererRef(new RendererGl()),
		  const SettingsFn &settingsFn = SettingsFn());

	  static void	cleanupLaunch();
	  bool			tickable() const;
	  void			tick();
};

template<class App>
void AppMsw::prepareLaunch(
	const std::string& title,
	const RendererRef &renderer /*= RendererRef(new RendererGl())*/,
	const SettingsFn &settingsFn /*= SettingsFn()*/)
{
	AppBase::prepareLaunch();

	Settings settings;
	AppMsw::initialize(&settings, renderer, title.c_str());

	if (settingsFn)
		settingsFn(&settings);

	if (!settings.getShouldQuit())
	{
		if (AppMsw *instance = dynamic_cast<AppMsw*>(new App))
		{
			instance->executeLaunch();
		}
	}
}

} } // namespace cinder::app
