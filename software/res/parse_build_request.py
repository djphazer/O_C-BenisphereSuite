import os

comment_text = os.environ['GH_COMMENT']

flags = comment_text.replace(',', ' ').replace(';', ' ').split()
custom_defines = "-DCUSTOM_BUILD"
pewcount = 0

for item in flags:
    f = item.strip().upper()
    if f.startswith('NOHEMI') or f.startswith('NO_HEM'):
        custom_defines += " -DNO_HEMISPHERE"
    if f.startswith('FLIP'):
        custom_defines += " -DFLIP_180"
    if f.startswith('VOR'):
        custom_defines += " -DVOR"
    if f.startswith('BUCHLA') or f.startswith('NLM') or f.startswith('NORTHERN'):
        custom_defines += " -DNORTHERNLIGHT"
    if f.startswith('NLM_HOC'):
        custom_defines += " -DNLM_hOC"
    if f.startswith('NLM_CARDOC'):
        custom_defines += " -DNLM_cardOC"
    if f.startswith('NLM_2OC_L'):
        custom_defines += " -DNORTHERNLIGHT_2OC_LEFTSIDE"
    if f.startswith('CALIBR8'):
        custom_defines += " -DENABLE_APP_CALIBR8OR"
    if f.startswith('SCENE'):
        custom_defines += " -DENABLE_APP_SCENES"
    if f.startswith('PONG'):
        custom_defines += " -DENABLE_APP_PONG"
    if f.startswith('ENIGMA'):
        custom_defines += " -DENABLE_APP_ENIGMA"
    if f.startswith('CAPTAIN') or f.startswith('MIDI'):
        custom_defines += " -DENABLE_APP_MIDI"
    if f.startswith('PIQUED'):
        custom_defines += " -DENABLE_APP_PIQUED"
    if f.startswith('QUADRAT'):
        custom_defines += " -DENABLE_APP_POLYLFO"
    if f.startswith('HARRING'):
        custom_defines += " -DENABLE_APP_H1200"
    if f.startswith('BYTE') or f.startswith('VIZNUT'):
        custom_defines += " -DENABLE_APP_BYTEBEATGEN"
    if f.startswith('NEURAL'):
        custom_defines += " -DENABLE_APP_NEURAL_NETWORK"
    if f.startswith('DARKEST'):
        custom_defines += " -DENABLE_APP_DARKEST_TIMELINE"
    if f.startswith('LOW-RENT') or f.startswith('LORENZ'):
        custom_defines += " -DENABLE_APP_LORENZ"
    if f.startswith('COPIER'):
        custom_defines += " -DENABLE_APP_ASR"
    if f.startswith('QUANTER'):
        custom_defines += " -DENABLE_APP_QUANTERMAIN"
    if f.startswith('META'):
        custom_defines += " -DENABLE_APP_METAQ"
    if f.startswith('ACID'):
        custom_defines += " -DENABLE_APP_CHORDS"
    if f.startswith('PASSEN'):
        custom_defines += " -DENABLE_APP_PASSENCORE"
    if f.startswith('SEQUIN'):
        custom_defines += " -DENABLE_APP_SEQUINS"
    if f.startswith('AUTOMATON'):
        custom_defines += " -DENABLE_APP_AUTOMATONNETZ"
    if f.startswith('DIALECT') or f.startswith('BBGEN'):
        custom_defines += " -DENABLE_APP_BBGEN"
    if f.startswith('REFS') or f.startswith('REFERENCE'):
        custom_defines += " -DENABLE_APP_REFERENCES"
    if f.startswith('GRIDS2'):
        custom_defines += " -DDRUMMAP_GRIDS2"
    if f.startswith('MOAR_PRESETS'):
        custom_defines += " -DMOAR_PRESETS"
    if f.startswith('PEWPEWPEW'):
        custom_defines += " -DPEWPEWPEW"
    elif f.startswith('PEW'):
        pewcount += 1

    #TODO proper lookup table

if pewcount > 2:
    custom_defines += " -DPEWPEWPEW"

print(custom_defines)
os.environ["CUSTOM_BUILD_FLAGS"] = custom_defines
