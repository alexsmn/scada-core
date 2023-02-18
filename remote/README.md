# JavaScript

Generate JS proto:

```batch
d:\vcpkg\installed\x64-windows\tools\protobuf\protoc --grpc_out=. --ts_out=. --js_out=. --plugin=protoc-gen-grpc=d:\vcpkg\installed\x64-windows\tools\grpc\grpc_node_plugin.exe scada.proto
```
