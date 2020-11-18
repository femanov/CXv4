# A setup file for term-kls exclusively:
#     add the koi8/75dpi dir and force use of 18px fonts

FONTS_DIR=/export/ctlhomes/rpi/fonts_KOI8-R_75dpi

( xset q | grep -L $FONTS_DIR >/dev/null)  ||  xset fp+ $FONTS_DIR

echo '*fontList: lucidasans-18' | xrdb -merge
echo '*.XmText.fontList: lucidasanstypewriter-18' | xrdb -merge
