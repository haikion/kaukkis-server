#include "server.h"
#include "message.h"
#include <QKeySequence>
#include <X11/keysym.h>

Server::Server(QObject* parent) :
    QObject(parent),
    socket_(new QTcpSocket(this)),
    //Finds out message size. It does not depend on message content.
    messageSize_(Message(Message::Action::KEY_PRESS, Qt::Key_0).serialize().size()),
    display_(XOpenDisplay(NULL))
{
    if (display_ == NULL)
    {
        qDebug() << "Server: Error: unable to open display!";
        exit(1);
    }
    qDebug() << "Server: message size: " << messageSize_;
    connect(&server_, SIGNAL(newConnection()),
            this, SLOT(newConnection()));

}

Server::~Server()
{
    XFlush(display_);
    XCloseDisplay(display_);
}

bool Server::listen(const unsigned port)
{
    if ( server_.listen(QHostAddress::Any, port) )
    {
        qDebug() << "Server: listening port: " << port;
        return true;
    }
    qDebug() << "Server could not start because: " + server_.errorString();
    return false;
}

//Called when new connection appears
void Server::newConnection()
{
    qDebug() << "Server: newConnection() executed";
    if (socket_->state() != QAbstractSocket::UnconnectedState)
    {
        //Close previous connection.
        socket_->close();
    }
    //Verifies that only one thread is reading the message.
    newConnectionMutex_.lock();
    delete socket_; //Delete old socket
    //Grab the new connection
    socket_ = server_.nextPendingConnection();
    socket_->waitForConnected();
    qDebug() << "Server: new connection from" << socket_->peerAddress().toString();

    //Reads until client disconnects or new connection appears.
    while (socket_->state() == QAbstractSocket::ConnectedState)
    {
        if (socket_->bytesAvailable() < messageSize_)
        {
            //Whole message has not been written.
            socket_->waitForReadyRead();
            continue; //Recheck if all bytes are written
        }

        //Deserialize the message
        QByteArray serializedMessage = socket_->read(messageSize_);
        Message msg(serializedMessage);

        //Run given action.
        unsigned keySym = QtKeyToXKeySym(msg.key());
        switch (msg.action()) {
            case Message::Action::KEY_PRESS:
                qDebug().nospace() << "Server: Key Press called. Key=" << QKeySequence(msg.key()).toString();
                pressKey(keySym);
                break;
            case Message::Action::KEY_RELEASE:
                qDebug().nospace() << "Server: Key Release called. Key=" << QKeySequence(msg.key()).toString();
                releaseKey(keySym);
                break;
        }
    }
    qDebug() << "Server: Client disconnected";
    newConnectionMutex_.unlock();
}

void Server::pressKey(const unsigned keyCode)
{
    unsigned keycode = XKeysymToKeycode(display_, keyCode);
    XTestFakeKeyEvent(display_, keycode, True, 0);
    XFlush(display_);
}

void Server::releaseKey(const unsigned keyCode)
{
    unsigned keycode = XKeysymToKeycode(display_, keyCode);
    XTestFakeKeyEvent(display_, keycode, false, 0);
    XFlush(display_);
}

unsigned Server::QtKeyToXKeySym(Qt::Key key)
{

    // Convert the Qt key code into an X keysym.
    KeySym keysym = NoSymbol;
    switch (key) {
        case Qt::Key_Escape: keysym = XK_Escape; break;
        case Qt::Key_Tab: keysym = XK_Tab; break;
        case Qt::Key_Backtab: keysym = XK_ISO_Left_Tab; break;
        case Qt::Key_Backspace: keysym = XK_BackSpace; break;
        case Qt::Key_Return: keysym = XK_Return; break;
        case Qt::Key_Enter: keysym = XK_KP_Enter; break;
        case Qt::Key_Insert: keysym = XK_KP_Insert; break;
        case Qt::Key_Delete: keysym = XK_KP_Delete; break;
        case Qt::Key_Pause: keysym = XK_Pause; break;
        case Qt::Key_Print: keysym = XK_Print; break;
        case Qt::Key_SysReq: keysym = 0x1005FF60; break;
        case Qt::Key_Clear: keysym = XK_KP_Begin; break;
        case Qt::Key_Home: keysym = XK_Home; break;
        case Qt::Key_End: keysym = XK_End; break;
        case Qt::Key_Left: keysym = XK_Left; break;
        case Qt::Key_Up: keysym = XK_Up; break;
        case Qt::Key_Right: keysym = XK_Right; break;
        case Qt::Key_Down: keysym = XK_Down; break;
        case Qt::Key_PageUp: keysym = XK_Prior; break;
        case Qt::Key_PageDown: keysym = XK_Next; break;
        case Qt::Key_Shift: keysym = XK_Shift_L; break;
        case Qt::Key_Control: keysym = XK_Control_L; break;
        case Qt::Key_Meta: keysym = XK_Meta_L; break;
        case Qt::Key_Alt: keysym = XK_Alt_L; break;
        case Qt::Key_CapsLock: keysym = XK_Caps_Lock; break;
        case Qt::Key_NumLock: keysym = XK_Num_Lock; break;
        case Qt::Key_ScrollLock: keysym = XK_Scroll_Lock; break;
        case Qt::Key_F1: keysym = XK_F1; break;
        case Qt::Key_F2: keysym = XK_F2; break;
        case Qt::Key_F3: keysym = XK_F3; break;
        case Qt::Key_F4: keysym = XK_F4; break;
        case Qt::Key_F5: keysym = XK_F5; break;
        case Qt::Key_F6: keysym = XK_F6; break;
        case Qt::Key_F7: keysym = XK_F7; break;
        case Qt::Key_F8: keysym = XK_F8; break;
        case Qt::Key_F9: keysym = XK_F9; break;
        case Qt::Key_F10: keysym = XK_F10; break;
        case Qt::Key_F11: keysym = XK_F11; break;
        case Qt::Key_F12: keysym = XK_F12; break;
        case Qt::Key_F13: keysym = XK_F13; break;
        case Qt::Key_F14: keysym = XK_F14; break;
        case Qt::Key_F15: keysym = XK_F15; break;
        case Qt::Key_F16: keysym = XK_F16; break;
        case Qt::Key_F17: keysym = XK_F17; break;
        case Qt::Key_F18: keysym = XK_F18; break;
        case Qt::Key_F19: keysym = XK_F19; break;
        case Qt::Key_F20: keysym = XK_F20; break;
        case Qt::Key_F21: keysym = XK_F21; break;
        case Qt::Key_F22: keysym = XK_F22; break;
        case Qt::Key_F23: keysym = XK_F23; break;
        case Qt::Key_F24: keysym = XK_F24; break;
        case Qt::Key_F25: keysym = XK_F25; break;
        case Qt::Key_F26: keysym = XK_F26; break;
        case Qt::Key_F27: keysym = XK_F27; break;
        case Qt::Key_F28: keysym = XK_F28; break;
        case Qt::Key_F29: keysym = XK_F29; break;
        case Qt::Key_F30: keysym = XK_F30; break;
        case Qt::Key_F31: keysym = XK_F31; break;
        case Qt::Key_F32: keysym = XK_F32; break;
        case Qt::Key_F33: keysym = XK_F33; break;
        case Qt::Key_F34: keysym = XK_F34; break;
        case Qt::Key_F35: keysym = XK_F35; break;
        case Qt::Key_Super_L: keysym = XK_Super_L; break;
        case Qt::Key_Super_R: keysym = XK_Super_R; break;
        case Qt::Key_Menu: keysym = XK_Menu; break;
        case Qt::Key_Hyper_L: keysym = XK_Hyper_L; break;
        case Qt::Key_Hyper_R: keysym = XK_Hyper_R; break;
        case Qt::Key_Help: keysym = XK_Help; break;
        case Qt::Key_Direction_L: keysym = NoSymbol; break; // ???
        case Qt::Key_Direction_R: keysym = NoSymbol; break; // ???
        case Qt::Key_Space: keysym = XK_space; break;
        case Qt::Key_Exclam: keysym = XK_exclam; break;
        case Qt::Key_QuoteDbl: keysym = XK_quotedbl; break;
        case Qt::Key_NumberSign: keysym = XK_numbersign; break;
        case Qt::Key_Dollar: keysym = XK_dollar; break;
        case Qt::Key_Percent: keysym = XK_percent; break;
        case Qt::Key_Ampersand: keysym = XK_ampersand; break;
        case Qt::Key_Apostrophe: keysym = XK_apostrophe; break;
        case Qt::Key_ParenLeft: keysym = XK_parenleft; break;
        case Qt::Key_ParenRight: keysym = XK_parenright; break;
        case Qt::Key_Asterisk: keysym = XK_asterisk; break;
        case Qt::Key_Plus: keysym = XK_plus; break;
        case Qt::Key_Comma: keysym = XK_comma; break;
        case Qt::Key_Minus: keysym = XK_minus; break;
        case Qt::Key_Period: keysym = XK_period; break;
        case Qt::Key_Slash: keysym = XK_slash; break;
        case Qt::Key_0: keysym = XK_0; break;
        case Qt::Key_1: keysym = XK_1; break;
        case Qt::Key_2: keysym = XK_2; break;
        case Qt::Key_3: keysym = XK_3; break;
        case Qt::Key_4: keysym = XK_4; break;
        case Qt::Key_5: keysym = XK_5; break;
        case Qt::Key_6: keysym = XK_6; break;
        case Qt::Key_7: keysym = XK_7; break;
        case Qt::Key_8: keysym = XK_8; break;
        case Qt::Key_9: keysym = XK_9; break;
        case Qt::Key_Colon: keysym = XK_colon; break;
        case Qt::Key_Semicolon: keysym = XK_semicolon; break;
        case Qt::Key_Less: keysym = XK_less; break;
        case Qt::Key_Equal: keysym = XK_equal; break;
        case Qt::Key_Greater: keysym = XK_greater; break;
        case Qt::Key_Question: keysym = XK_question; break;
        case Qt::Key_At: keysym = XK_at; break;
        case Qt::Key_A: keysym = XK_a; break; // Must be lower case keysyms
        case Qt::Key_B: keysym = XK_b; break; // for correct shift handling.
        case Qt::Key_C: keysym = XK_c; break;
        case Qt::Key_D: keysym = XK_d; break;
        case Qt::Key_E: keysym = XK_e; break;
        case Qt::Key_F: keysym = XK_f; break;
        case Qt::Key_G: keysym = XK_g; break;
        case Qt::Key_H: keysym = XK_h; break;
        case Qt::Key_I: keysym = XK_i; break;
        case Qt::Key_J: keysym = XK_j; break;
        case Qt::Key_K: keysym = XK_k; break;
        case Qt::Key_L: keysym = XK_l; break;
        case Qt::Key_M: keysym = XK_m; break;
        case Qt::Key_N: keysym = XK_n; break;
        case Qt::Key_O: keysym = XK_o; break;
        case Qt::Key_P: keysym = XK_p; break;
        case Qt::Key_Q: keysym = XK_q; break;
        case Qt::Key_R: keysym = XK_r; break;
        case Qt::Key_S: keysym = XK_s; break;
        case Qt::Key_T: keysym = XK_t; break;
        case Qt::Key_U: keysym = XK_u; break;
        case Qt::Key_V: keysym = XK_v; break;
        case Qt::Key_W: keysym = XK_w; break;
        case Qt::Key_X: keysym = XK_x; break;
        case Qt::Key_Y: keysym = XK_y; break;
        case Qt::Key_Z: keysym = XK_z; break;
        case Qt::Key_BracketLeft: keysym = XK_bracketleft; break;
        case Qt::Key_Backslash: keysym = XK_backslash; break;
        case Qt::Key_BracketRight: keysym = XK_bracketright; break;
        case Qt::Key_AsciiCircum: keysym = XK_asciicircum; break;
        case Qt::Key_Underscore: keysym = XK_underscore; break;
        case Qt::Key_QuoteLeft: keysym = XK_quoteleft; break;
        case Qt::Key_BraceLeft: keysym = XK_braceleft; break;
        case Qt::Key_Bar: keysym = XK_bar; break;
        case Qt::Key_BraceRight: keysym = XK_braceright; break;
        case Qt::Key_AsciiTilde: keysym = XK_asciitilde; break;

        case Qt::Key_nobreakspace: keysym = XK_nobreakspace; break;
        case Qt::Key_exclamdown: keysym = XK_exclamdown; break;
        case Qt::Key_cent: keysym = XK_cent; break;
        case Qt::Key_sterling: keysym = XK_sterling; break;
        case Qt::Key_currency: keysym = XK_currency; break;
        case Qt::Key_yen: keysym = XK_yen; break;
        case Qt::Key_brokenbar: keysym = XK_brokenbar; break;
        case Qt::Key_section: keysym = XK_section; break;
        case Qt::Key_diaeresis: keysym = XK_diaeresis; break;
        case Qt::Key_copyright: keysym = XK_copyright; break;
        case Qt::Key_ordfeminine: keysym = XK_ordfeminine; break;
        case Qt::Key_guillemotleft: keysym = XK_guillemotleft; break;
        case Qt::Key_notsign: keysym = XK_notsign; break;
        case Qt::Key_hyphen: keysym = XK_hyphen; break;
        case Qt::Key_registered: keysym = XK_registered; break;
        case Qt::Key_macron: keysym = XK_macron; break;
        case Qt::Key_degree: keysym = XK_degree; break;
        case Qt::Key_plusminus: keysym = XK_plusminus; break;
        case Qt::Key_twosuperior: keysym = XK_twosuperior; break;
        case Qt::Key_threesuperior: keysym = XK_threesuperior; break;
        case Qt::Key_acute: keysym = XK_acute; break;
        case Qt::Key_mu: keysym = XK_mu; break;
        case Qt::Key_paragraph: keysym = XK_paragraph; break;
        case Qt::Key_periodcentered: keysym = XK_periodcentered; break;
        case Qt::Key_cedilla: keysym = XK_cedilla; break;
        case Qt::Key_onesuperior: keysym = XK_onesuperior; break;
        case Qt::Key_masculine: keysym = XK_masculine; break;
        case Qt::Key_guillemotright: keysym = XK_guillemotright; break;
        case Qt::Key_onequarter: keysym = XK_onequarter; break;
        case Qt::Key_onehalf: keysym = XK_onehalf; break;
        case Qt::Key_threequarters: keysym = XK_threequarters; break;
        case Qt::Key_questiondown: keysym = XK_questiondown; break;
        case Qt::Key_Agrave: keysym = XK_agrave; break;	// Lower case keysyms
        case Qt::Key_Aacute: keysym = XK_aacute; break; // for shift handling.
        case Qt::Key_Acircumflex: keysym = XK_acircumflex; break;
        case Qt::Key_Atilde: keysym = XK_atilde; break;
        case Qt::Key_Adiaeresis: keysym = XK_adiaeresis; break;
        case Qt::Key_Aring: keysym = XK_aring; break;
        case Qt::Key_AE: keysym = XK_ae; break;
        case Qt::Key_Ccedilla: keysym = XK_ccedilla; break;
        case Qt::Key_Egrave: keysym = XK_egrave; break;
        case Qt::Key_Eacute: keysym = XK_eacute; break;
        case Qt::Key_Ecircumflex: keysym = XK_ecircumflex; break;
        case Qt::Key_Ediaeresis: keysym = XK_ediaeresis; break;
        case Qt::Key_Igrave: keysym = XK_igrave; break;
        case Qt::Key_Iacute: keysym = XK_iacute; break;
        case Qt::Key_Icircumflex: keysym = XK_icircumflex; break;
        case Qt::Key_Idiaeresis: keysym = XK_idiaeresis; break;
        case Qt::Key_ETH: keysym = XK_eth; break;
        case Qt::Key_Ntilde: keysym = XK_ntilde; break;
        case Qt::Key_Ograve: keysym = XK_ograve; break;
        case Qt::Key_Oacute: keysym = XK_oacute; break;
        case Qt::Key_Ocircumflex: keysym = XK_ocircumflex; break;
        case Qt::Key_Otilde: keysym = XK_otilde; break;
        case Qt::Key_Odiaeresis: keysym = XK_odiaeresis; break;
        case Qt::Key_multiply: keysym = XK_multiply; break;
        case Qt::Key_Ooblique: keysym = XK_ooblique; break;
        case Qt::Key_Ugrave: keysym = XK_ugrave; break;
        case Qt::Key_Uacute: keysym = XK_uacute; break;
        case Qt::Key_Ucircumflex: keysym = XK_ucircumflex; break;
        case Qt::Key_Udiaeresis: keysym = XK_udiaeresis; break;
        case Qt::Key_Yacute: keysym = XK_yacute; break;
        case Qt::Key_THORN: keysym = XK_thorn; break;
        case Qt::Key_ssharp: keysym = XK_ssharp; break;
        case Qt::Key_division: keysym = XK_division; break;
        case Qt::Key_ydiaeresis: keysym = XK_ydiaeresis; break;

        case Qt::Key_AltGr: keysym = XK_ISO_Level3_Shift; break;
        case Qt::Key_Multi_key: keysym = XK_Multi_key; break;
        case Qt::Key_Codeinput: keysym = XK_Codeinput; break;
        case Qt::Key_SingleCandidate: keysym = XK_SingleCandidate; break;
        case Qt::Key_MultipleCandidate: keysym = XK_MultipleCandidate; break;
        case Qt::Key_PreviousCandidate: keysym = XK_PreviousCandidate; break;

        case Qt::Key_Mode_switch: keysym = XK_Mode_switch; break;
        case Qt::Key_Kanji: keysym = XK_Kanji; break;
        case Qt::Key_Muhenkan: keysym = XK_Muhenkan; break;
        case Qt::Key_Henkan: keysym = XK_Henkan; break;
        case Qt::Key_Romaji: keysym = XK_Romaji; break;
        case Qt::Key_Hiragana: keysym = XK_Hiragana; break;
        case Qt::Key_Katakana: keysym = XK_Katakana; break;
        case Qt::Key_Hiragana_Katakana: keysym = XK_Hiragana_Katakana; break;
        case Qt::Key_Zenkaku: keysym = XK_Zenkaku; break;
        case Qt::Key_Hankaku: keysym = XK_Hankaku; break;
        case Qt::Key_Zenkaku_Hankaku: keysym = XK_Zenkaku_Hankaku; break;
        case Qt::Key_Touroku: keysym = XK_Touroku; break;
        case Qt::Key_Massyo: keysym = XK_Massyo; break;
        case Qt::Key_Kana_Lock: keysym = XK_Kana_Lock; break;
        case Qt::Key_Kana_Shift: keysym = XK_Kana_Shift; break;
        case Qt::Key_Eisu_Shift: keysym = XK_Eisu_Shift; break;
        case Qt::Key_Eisu_toggle: keysym = XK_Eisu_toggle; break;

        case Qt::Key_Hangul: keysym = XK_Hangul; break;
        case Qt::Key_Hangul_Start: keysym = XK_Hangul_Start; break;
        case Qt::Key_Hangul_End: keysym = XK_Hangul_End; break;
        case Qt::Key_Hangul_Hanja: keysym = XK_Hangul_Hanja; break;
        case Qt::Key_Hangul_Jamo: keysym = XK_Hangul_Jamo; break;
        case Qt::Key_Hangul_Romaja: keysym = XK_Hangul_Romaja; break;
        case Qt::Key_Hangul_Jeonja: keysym = XK_Hangul_Jeonja; break;
        case Qt::Key_Hangul_Banja: keysym = XK_Hangul_Banja; break;
        case Qt::Key_Hangul_PreHanja: keysym = XK_Hangul_PreHanja; break;
        case Qt::Key_Hangul_PostHanja: keysym = XK_Hangul_PostHanja; break;
        case Qt::Key_Hangul_Special: keysym = XK_Hangul_Special; break;

        case Qt::Key_Dead_Grave: keysym = XK_dead_grave; break;
        case Qt::Key_Dead_Acute: keysym = XK_dead_acute; break;
        case Qt::Key_Dead_Circumflex: keysym = XK_dead_circumflex; break;
        case Qt::Key_Dead_Tilde: keysym = XK_dead_tilde; break;
        case Qt::Key_Dead_Macron: keysym = XK_dead_macron; break;
        case Qt::Key_Dead_Breve: keysym = XK_dead_breve; break;
        case Qt::Key_Dead_Abovedot: keysym = XK_dead_abovedot; break;
        case Qt::Key_Dead_Diaeresis: keysym = XK_dead_diaeresis; break;
        case Qt::Key_Dead_Abovering: keysym = XK_dead_abovering; break;
        case Qt::Key_Dead_Doubleacute: keysym = XK_dead_doubleacute; break;
        case Qt::Key_Dead_Caron: keysym = XK_dead_caron; break;
        case Qt::Key_Dead_Cedilla: keysym = XK_dead_cedilla; break;
        case Qt::Key_Dead_Ogonek: keysym = XK_dead_ogonek; break;
        case Qt::Key_Dead_Iota: keysym = XK_dead_iota; break;
        case Qt::Key_Dead_Voiced_Sound: keysym = XK_dead_voiced_sound; break;
        case Qt::Key_Dead_Semivoiced_Sound: keysym = XK_dead_semivoiced_sound; break;
        case Qt::Key_Dead_Belowdot: keysym = XK_dead_belowdot; break;
        case Qt::Key_Dead_Hook: keysym = XK_dead_hook; break;
        case Qt::Key_Dead_Horn: keysym = XK_dead_horn; break;
        /*
        case Qt::Key_Back: keysym = XF86XK_Back; break;
        case Qt::Key_Forward: keysym = XF86XK_Forward; break;
        case Qt::Key_Stop: keysym = XF86XK_Stop; break;
        case Qt::Key_Refresh: keysym = XF86XK_Refresh; break;

        case Qt::Key_VolumeDown: keysym = XF86XK_AudioLowerVolume; break;
        case Qt::Key_VolumeMute: keysym = XF86XK_AudioMute; break;
        case Qt::Key_VolumeUp: keysym = XF86XK_AudioRaiseVolume; break;
        case Qt::Key_BassBoost: keysym = NoSymbol; break;	// ???
        case Qt::Key_BassUp: keysym = NoSymbol; break;		// ???
        case Qt::Key_BassDown: keysym = NoSymbol; break;	// ???
        case Qt::Key_TrebleUp: keysym = NoSymbol; break;	// ???
        case Qt::Key_TrebleDown: keysym = NoSymbol; break;	// ???

        case Qt::Key_MediaPlay: keysym = XF86XK_AudioPlay; break;
        case Qt::Key_MediaStop: keysym = XF86XK_AudioStop; break;
        case Qt::Key_MediaPrevious: keysym = XF86XK_AudioPrev; break;
        case Qt::Key_MediaNext: keysym = XF86XK_AudioNext; break;
        case Qt::Key_MediaRecord: keysym = XF86XK_AudioRecord; break;

        case Qt::Key_HomePage: keysym = XF86XK_HomePage; break;
        case Qt::Key_Favorites: keysym = XF86XK_Favorites; break;
        case Qt::Key_Search: keysym = XF86XK_Search; break;
        case Qt::Key_Standby: keysym = XF86XK_Standby; break;
        case Qt::Key_OpenUrl: keysym = XF86XK_OpenURL; break;

        case Qt::Key_LaunchMail: keysym = XF86XK_Mail; break;
        case Qt::Key_LaunchMedia: keysym = XF86XK_AudioMedia; break;
        case Qt::Key_Launch0: keysym = XF86XK_Launch0; break;
        case Qt::Key_Launch1: keysym = XF86XK_Launch1; break;
        case Qt::Key_Launch2: keysym = XF86XK_Launch2; break;
        case Qt::Key_Launch3: keysym = XF86XK_Launch3; break;
        case Qt::Key_Launch4: keysym = XF86XK_Launch4; break;
        case Qt::Key_Launch5: keysym = XF86XK_Launch5; break;
        case Qt::Key_Launch6: keysym = XF86XK_Launch6; break;
        case Qt::Key_Launch7: keysym = XF86XK_Launch7; break;
        case Qt::Key_Launch8: keysym = XF86XK_Launch8; break;
        case Qt::Key_Launch9: keysym = XF86XK_Launch9; break;
        case Qt::Key_LaunchA: keysym = XF86XK_LaunchA; break;
        case Qt::Key_LaunchB: keysym = XF86XK_LaunchB; break;
        case Qt::Key_LaunchC: keysym = XF86XK_LaunchC; break;
        case Qt::Key_LaunchD: keysym = XF86XK_LaunchD; break;
        case Qt::Key_LaunchE: keysym = XF86XK_LaunchE; break;
        case Qt::Key_LaunchF: keysym = XF86XK_LaunchF; break;

        case Qt::Key_MediaLast: keysym = NoSymbol; break;   // ???

        case Qt::Key_Select: keysym = QTOPIAXK_Select; break;
        case Qt::Key_Yes: keysym = QTOPIAXK_Yes; break;
        case Qt::Key_No: keysym = QTOPIAXK_No; break;

        case Qt::Key_Cancel: keysym = QTOPIAXK_Cancel; break;
        case Qt::Key_Printer: keysym = QTOPIAXK_Printer; break;
        case Qt::Key_Execute: keysym = QTOPIAXK_Execute; break;
        case Qt::Key_Sleep: keysym = QTOPIAXK_Sleep; break;
        case Qt::Key_Play: keysym = QTOPIAXK_Play; break;
        case Qt::Key_Zoom: keysym = QTOPIAXK_Zoom; break;

        case Qt::Key_Context1: keysym = QTOPIAXK_Context1; break;
        case Qt::Key_Context2: keysym = QTOPIAXK_Context2; break;
        case Qt::Key_Context3: keysym = QTOPIAXK_Context3; break;
        case Qt::Key_Context4: keysym = QTOPIAXK_Context4; break;
        case Qt::Key_Call: keysym = QTOPIAXK_Call; break;
        case Qt::Key_Hangup: keysym = QTOPIAXK_Hangup; break;
        case Qt::Key_Flip: keysym = QTOPIAXK_Flip; break;
        */
        default:
            qDebug() << "Server: Undetected key: " << QKeySequence(key).toString();
            break;
    }
    return keysym;
}
