set LOCALES=ru

python make_translations.py data_items_model.xml data_items_translations.json %LOCALES%
python make_translations.py devices_model.xml devices_translations.json %LOCALES%
python make_translations.py security_model.xml security_translations.json %LOCALES%
python make_translations.py scada_model.xml scada_translations.json %LOCALES%
python make_translations.py filesystem_model.xml filesystem_translations.json %LOCALES%
python make_translations.py history_model.xml history_translations.json %LOCALES%
