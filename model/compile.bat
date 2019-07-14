set COMPILER=d:\ThirdParty\UA-ModelCompiler\Bin\Release\Opc.Ua.ModelCompiler.exe

%COMPILER% -d2 scada_model.xml -c scada_model.csv -o2 generated -console
%COMPILER% -d2 security_model.xml -c security_model.csv -o2 generated -console
%COMPILER% -d2 data_items_model.xml -c data_items_model.csv -o2 generated -console
%COMPILER% -d2 history_model.xml -c history_model.csv -o2 generated -console
%COMPILER% -d2 devices_model.xml -c devices_model.csv -o2 generated -console
%COMPILER% -d2 filesystem_model.xml -c filesystem_model.csv -o2 generated -console

python compile_node_ids.py scada_model.csv scada > scada_node_ids.h
python compile_node_ids.py security_model.csv security > security_node_ids.h
python compile_node_ids.py data_items_model.csv data_items > data_items_node_ids.h
python compile_node_ids.py history_model.csv history > history_node_ids.h
python compile_node_ids.py devices_model.csv devices > devices_node_ids.h
python compile_node_ids.py filesystem_model.csv filesystem > filesystem_node_ids.h