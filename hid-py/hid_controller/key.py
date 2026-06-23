"""Virtual key codes for hid_send.key_down(), key_up(), key_press(), key_combo(), key_seq().

Values match Windows Virtual-Key constants.

Examples
--------
>>> from hid_controller import Key
>>> from hid_controller.hid_send import key_press, key_down
>>> key_press(Key.ENTER)
>>> key_down(Key.LCTRL)
"""

from enum import IntEnum


class Key(IntEnum):
    """Virtual key codes.

    Use with hid_send.key_down(), key_up(), key_press(), key_combo(), key_seq().
    String names like ``"A"``, ``"enter"``, ``"lctrl"`` are still supported.

    Attributes
    ----------
    LBUTTON : int
        Left mouse button (0x01).
    RBUTTON : int
        Right mouse button (0x02).
    MBUTTON : int
        Middle mouse button (0x04).
    BACK : int
        Backspace (0x08).
    TAB : int
        Tab (0x09).
    ENTER : int
        Enter / Return (0x0D).
    SHIFT : int
        Shift key (0x10).
    CTRL : int
        Ctrl key (0x11).
    ALT : int
        Alt key (0x12).
    PAUSE : int
        Pause (0x13).
    CAPS_LOCK : int
        Caps Lock (0x14).
    IME_KANA : int
        IME Kana mode (0x15).
    IME_HANGUL : int
        IME Hangul mode (0x15).
    JUNJA : int
        IME Junja mode (0x17).
    FINAL : int
        IME Final mode (0x18).
    HANJA : int
        IME Hanja mode (0x19).
    IME_CONVERT : int
        IME Convert (0x1C).
    IME_NONCONVERT : int
        IME Non-convert (0x1D).
    IME_ACCEPT : int
        IME Accept (0x1E).
    IME_MODECHANGE : int
        IME Mode change (0x1F).
    SPACE : int
        Space bar (0x20).
    PRIOR : int
        Page Up (0x21).
    NEXT : int
        Page Down (0x22).
    END : int
        End (0x23).
    HOME : int
        Home (0x24).
    LEFT : int
        Left arrow (0x25).
    UP : int
        Up arrow (0x26).
    RIGHT : int
        Right arrow (0x27).
    DOWN : int
        Down arrow (0x28).
    SELECT : int
        Select (0x29).
    PRINT : int
        Print screen (0x2A).
    EXECUTE : int
        Execute (0x2B).
    SNAPSHOT : int
        Snapshot (0x2C).
    INSERT : int
        Insert (0x2D).
    DELETE : int
        Delete (0x2E).
    HELP : int
        Help (0x2F).
    NUM0 : int
        0 (0x30).
    NUM1 : int
        1 (0x31).
    NUM2 : int
        2 (0x32).
    NUM3 : int
        3 (0x33).
    NUM4 : int
        4 (0x34).
    NUM5 : int
        5 (0x35).
    NUM6 : int
        6 (0x36).
    NUM7 : int
        7 (0x37).
    NUM8 : int
        8 (0x38).
    NUM9 : int
        9 (0x39).
    A : int
        A (0x41).
    B : int
        B (0x42).
    C : int
        C (0x43).
    D : int
        D (0x44).
    E : int
        E (0x45).
    F : int
        F (0x46).
    G : int
        G (0x47).
    H : int
        H (0x48).
    I : int
        I (0x49).
    J : int
        J (0x4A).
    K : int
        K (0x4B).
    L : int
        L (0x4C).
    M : int
        M (0x4D).
    N : int
        N (0x4E).
    O : int
        O (0x4F).
    P : int
        P (0x50).
    Q : int
        Q (0x51).
    R : int
        R (0x52).
    S : int
        S (0x53).
    T : int
        T (0x54).
    U : int
        U (0x55).
    V : int
        V (0x56).
    W : int
        W (0x57).
    X : int
        X (0x58).
    Y : int
        Y (0x59).
    Z : int
        Z (0x5A).
    LWIN : int
        Left Windows key (0x5B).
    RWIN : int
        Right Windows key (0x5C).
    APPS : int
        Applications key (0x5D).
    SLEEP : int
        Sleep (0x5F).
    NUMPAD0 : int
        Numpad 0 (0x60).
    NUMPAD1 : int
        Numpad 1 (0x61).
    NUMPAD2 : int
        Numpad 2 (0x62).
    NUMPAD3 : int
        Numpad 3 (0x63).
    NUMPAD4 : int
        Numpad 4 (0x64).
    NUMPAD5 : int
        Numpad 5 (0x65).
    NUMPAD6 : int
        Numpad 6 (0x66).
    NUMPAD7 : int
        Numpad 7 (0x67).
    NUMPAD8 : int
        Numpad 8 (0x68).
    NUMPAD9 : int
        Numpad 9 (0x69).
    MULTIPLY : int
        Multiply (0x6A).
    ADD : int
        Add (0x6B).
    SEPARATOR : int
        Separator (0x6C).
    SUBTRACT : int
        Subtract (0x6D).
    DECIMAL : int
        Decimal (0x6E).
    DIVIDE : int
        Divide (0x6F).
    F1 : int
        F1 (0x70).
    F2 : int
        F2 (0x71).
    F3 : int
        F3 (0x72).
    F4 : int
        F4 (0x73).
    F5 : int
        F5 (0x74).
    F6 : int
        F6 (0x75).
    F7 : int
        F7 (0x76).
    F8 : int
        F8 (0x77).
    F9 : int
        F9 (0x78).
    F10 : int
        F10 (0x79).
    F11 : int
        F11 (0x7A).
    F12 : int
        F12 (0x7B).
    F13 : int
        F13 (0x7C).
    F14 : int
        F14 (0x7D).
    F15 : int
        F15 (0x7E).
    F16 : int
        F16 (0x7F).
    F17 : int
        F17 (0x80).
    F18 : int
        F18 (0x81).
    F19 : int
        F19 (0x82).
    F20 : int
        F20 (0x83).
    F21 : int
        F21 (0x84).
    F22 : int
        F22 (0x85).
    F23 : int
        F23 (0x86).
    F24 : int
        F24 (0x87).
    NUM_LOCK : int
        Num Lock (0x90).
    SCROLL_LOCK : int
        Scroll Lock (0x91).
    LSHIFT : int
        Left Shift (0xA0).
    RSHIFT : int
        Right Shift (0xA1).
    LCTRL : int
        Left Ctrl (0xA2).
    RCTRL : int
        Right Ctrl (0xA3).
    LALT : int
        Left Alt (0xA4).
    RALT : int
        Right Alt (0xA5).
    BROWSER_BACK : int
        Browser Back (0xA6).
    BROWSER_FORWARD : int
        Browser Forward (0xA7).
    BROWSER_REFRESH : int
        Browser Refresh (0xA8).
    BROWSER_STOP : int
        Browser Stop (0xA9).
    BROWSER_SEARCH : int
        Browser Search (0xAA).
    BROWSER_FAVORITES : int
        Browser Favorites (0xAB).
    BROWSER_HOME : int
        Browser Home (0xAC).
    VOLUME_MUTE : int
        Volume Mute (0xAD).
    VOLUME_DOWN : int
        Volume Down (0xAE).
    VOLUME_UP : int
        Volume Up (0xAF).
    MEDIA_NEXT_TRACK : int
        Media Next Track (0xB0).
    MEDIA_PREV_TRACK : int
        Media Prev Track (0xB1).
    MEDIA_STOP : int
        Media Stop (0xB2).
    MEDIA_PLAY_PAUSE : int
        Media Play/Pause (0xB3).
    LAUNCH_MAIL : int
        Launch Mail (0xB4).
    LAUNCH_MEDIA_SELECT : int
        Media Select (0xB5).
    LAUNCH_APP1 : int
        Launch App1 (0xB6).
    LAUNCH_APP2 : int
        Launch App2 (0xB7).
    OEM_1 : int
        ;: (0xBA).
    OEM_PLUS : int
        =+ (0xBB).
    OEM_COMMA : int
        ,< (0xBC).
    OEM_MINUS : int
        -_ (0xBD).
    OEM_PERIOD : int
        .> (0xBE).
    OEM_2 : int
        ?/ (0xBF).
    OEM_3 : int
        `~ (0xC0).
    OEM_4 : int
        [{ (0xDB).
    OEM_5 : int
        \\| (0xDC).
    OEM_6 : int
        ]} (0xDD).
    OEM_7 : int
        '" (0xDE).
    OEM_8 : int
        (0xDF).
    OEM_102 : int
        <> (0xE2).
    PROCESSKEY : int
        Process (0xE5).
    OEM_CLEAR : int
        OEM Clear (0xE3).
    PACKET : int
        Packet (0xE7).
    UNASSIGNED : int
        Unassigned (0xE9).
    ATTN : int
        Attn (0xF6).
    CRSEL : int
        CrSel (0xF7).
    EXSEL : int
        ExSel (0xF8).
    EREOF : int
        Erase EOF (0xF9).
    PLAY : int
        Play (0xFA).
    ZOOM : int
        Zoom (0xFB).
    PA1 : int
        Pa1 (0xFD).
"""

    # ---- mouse buttons (also usable with key_combo/seq on some drivers) ----
    LBUTTON = 0x01
    RBUTTON = 0x02
    MBUTTON = 0x04
    XBUTTON1 = 0x05
    XBUTTON2 = 0x06

    # ---- control keys ----
    BACK = 0x08
    TAB = 0x09
    ENTER = 0x0D
    SHIFT = 0x10
    CTRL = 0x11
    ALT = 0x12
    PAUSE = 0x13
    CAPS_LOCK = 0x14

    # ---- IME keys ----
    IME_KANA = 0x15
    IME_HANGUL = 0x15
    JUNJA = 0x17
    FINAL = 0x18
    HANJA = 0x19
    IME_CONVERT = 0x1C
    IME_NONCONVERT = 0x1D
    IME_ACCEPT = 0x1E
    IME_MODECHANGE = 0x1F

    # ---- navigation ----
    SPACE = 0x20
    PRIOR = 0x21
    NEXT = 0x22
    END = 0x23
    HOME = 0x24
    LEFT = 0x25
    UP = 0x26
    RIGHT = 0x27
    DOWN = 0x28
    SELECT = 0x29
    PRINT = 0x2A
    EXECUTE = 0x2B
    SNAPSHOT = 0x2C
    INSERT = 0x2D
    DELETE = 0x2E
    HELP = 0x2F

    # ---- 0-9 ----
    NUM0 = 0x30
    NUM1 = 0x31
    NUM2 = 0x32
    NUM3 = 0x33
    NUM4 = 0x34
    NUM5 = 0x35
    NUM6 = 0x36
    NUM7 = 0x37
    NUM8 = 0x38
    NUM9 = 0x39

    # ---- A-Z ----
    A = 0x41
    B = 0x42
    C = 0x43
    D = 0x44
    E = 0x45
    F = 0x46
    G = 0x47
    H = 0x48
    I = 0x49
    J = 0x4A
    K = 0x4B
    L = 0x4C
    M = 0x4D
    N = 0x4E
    O = 0x4F
    P = 0x50
    Q = 0x51
    R = 0x52
    S = 0x53
    T = 0x54
    U = 0x55
    V = 0x56
    W = 0x57
    X = 0x58
    Y = 0x59
    Z = 0x5A

    # ---- modifier / special ----
    LWIN = 0x5B
    RWIN = 0x5C
    APPS = 0x5D
    SLEEP = 0x5F

    # ---- numpad ----
    NUMPAD0 = 0x60
    NUMPAD1 = 0x61
    NUMPAD2 = 0x62
    NUMPAD3 = 0x63
    NUMPAD4 = 0x64
    NUMPAD5 = 0x65
    NUMPAD6 = 0x66
    NUMPAD7 = 0x67
    NUMPAD8 = 0x68
    NUMPAD9 = 0x69
    MULTIPLY = 0x6A
    ADD = 0x6B
    SEPARATOR = 0x6C
    SUBTRACT = 0x6D
    DECIMAL = 0x6E
    DIVIDE = 0x6F

    # ---- function keys ----
    F1 = 0x70
    F2 = 0x71
    F3 = 0x72
    F4 = 0x73
    F5 = 0x74
    F6 = 0x75
    F7 = 0x76
    F8 = 0x77
    F9 = 0x78
    F10 = 0x79
    F11 = 0x7A
    F12 = 0x7B
    F13 = 0x7C
    F14 = 0x7D
    F15 = 0x7E
    F16 = 0x7F
    F17 = 0x80
    F18 = 0x81
    F19 = 0x82
    F20 = 0x83
    F21 = 0x84
    F22 = 0x85
    F23 = 0x86
    F24 = 0x87

    # ---- status ----
    NUM_LOCK = 0x90
    SCROLL_LOCK = 0x91

    # ---- shift ----
    LSHIFT = 0xA0
    RSHIFT = 0xA1

    # ---- ctrl ----
    LCTRL = 0xA2
    RCTRL = 0xA3

    # ---- alt ----
    LALT = 0xA4
    RALT = 0xA5

    # ---- browser ----
    BROWSER_BACK = 0xA6
    BROWSER_FORWARD = 0xA7
    BROWSER_REFRESH = 0xA8
    BROWSER_STOP = 0xA9
    BROWSER_SEARCH = 0xAA
    BROWSER_FAVORITES = 0xAB
    BROWSER_HOME = 0xAC

    # ---- media ----
    VOLUME_MUTE = 0xAD
    VOLUME_DOWN = 0xAE
    VOLUME_UP = 0xAF
    MEDIA_NEXT_TRACK = 0xB0
    MEDIA_PREV_TRACK = 0xB1
    MEDIA_STOP = 0xB2
    MEDIA_PLAY_PAUSE = 0xB3
    LAUNCH_MAIL = 0xB4
    LAUNCH_MEDIA_SELECT = 0xB5
    LAUNCH_APP1 = 0xB6
    LAUNCH_APP2 = 0xB7

    # ---- OEM ----
    OEM_1 = 0xBA
    OEM_PLUS = 0xBB
    OEM_COMMA = 0xBC
    OEM_MINUS = 0xBD
    OEM_PERIOD = 0xBE
    OEM_2 = 0xBF
    OEM_3 = 0xC0
    OEM_4 = 0xDB
    OEM_5 = 0xDC
    OEM_6 = 0xDD
    OEM_7 = 0xDE
    OEM_8 = 0xDF
    OEM_102 = 0xE2
    PROCESSKEY = 0xE5
    OEM_CLEAR = 0xE3
    PACKET = 0xE7
    UNASSIGNED = 0xE9

    # ---- extended ----
    ATTN = 0xF6
    CRSEL = 0xF7
    EXSEL = 0xF8
    EREOF = 0xF9
    PLAY = 0xFA
    ZOOM = 0xFB
    PA1 = 0xFD
