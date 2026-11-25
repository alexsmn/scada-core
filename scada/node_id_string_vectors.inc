// Test vectors for the OPC UA Part 6 §5.1.12 NodeId string form ("nsu="),
// https://reference.opcfoundation.org/Core/Part6/v105/docs/5.1.12 .
//
// Two products parse and write this form with their own code — core
// (scada::ParseNodeIdString) and the Designer, which may not consume core
// (server_link::ParseNodeIdString). Each keeps an identical copy of this file
// beside its parser and runs its test over it;
// tools/build/check_node_id_string_vectors.py fails when the copies differ.
// Edit both together.
//
// The includer defines the four macros. Identifier kinds: 'i' (value is the
// decimal), 's' (the string), 'g' (the Guid, upper-case 8-4-4-4-12), 'b' (the
// opaque id's bytes, as upper-case hex).
//
//   NODE_ID_STRING_CANONICAL(text, uri, kind, value)
//       Written exactly as `text` from (uri, identifier), and read back.
//   NODE_ID_STRING_WRITTEN(text, uri, kind, value)
//       Written as `text`; reading it gives another URI (the bare form).
//   NODE_ID_STRING_READ(text, uri, ns, kind, value)
//       Read as (uri, "ns=" index, identifier); not what the writer writes.
//   NODE_ID_STRING_REJECTED(text)
//       Not exactly one of the forms: the parser returns nothing.

// Three come from Part 6 §5.1.12's own examples.
NODE_ID_STRING_CANONICAL("i=13", "", 'i', "13")
NODE_ID_STRING_CANONICAL("i=2258", "", 'i', "2258")
NODE_ID_STRING_CANONICAL(
    "nsu=urn:open62541.server.application;s=Plant.Temperature",
    "urn:open62541.server.application", 's', "Plant.Temperature")
NODE_ID_STRING_CANONICAL("nsu=http://widgets.com/schemas/hello;s=水 World",
                         "http://widgets.com/schemas/hello", 's', "水 World")
NODE_ID_STRING_CANONICAL(
    "nsu=tag:acme.com,2023:schemas:data#off%3B;b=M/RbKBsRVkePCePcx24oRA==",
    "tag:acme.com,2023:schemas:data#off;", 'b',
    "33F45B281B1156478F09E3DCC76E2844")
NODE_ID_STRING_CANONICAL(
    "nsu=http://telecontrol.ru/opcua/data_items/DiscreteItemType;i=1379",
    "http://telecontrol.ru/opcua/data_items/DiscreteItemType", 'i', "1379")
NODE_ID_STRING_CANONICAL("nsu=urn:x;g=09087E75-8E5E-499B-954F-F2A9603DB28A",
                         "urn:x", 'g', "09087E75-8E5E-499B-954F-F2A9603DB28A")
NODE_ID_STRING_CANONICAL("nsu=urn:x;b=", "urn:x", 'b', "")
NODE_ID_STRING_CANONICAL("nsu=urn:x;b=AQ==", "urn:x", 'b', "01")
NODE_ID_STRING_CANONICAL("nsu=urn:x;b=AQI=", "urn:x", 'b', "0102")
// A namespace "URI" that is not one: every byte outside RFC 3986 is escaped,
// '%' and '}' included, so a braced formula never ends early.
NODE_ID_STRING_CANONICAL("nsu=my%20plant%7Bv%25%7D;s=a", "my plant{v%}", 's',
                         "a")
NODE_ID_STRING_CANONICAL("nsu=urn:x;s=", "urn:x", 's', "")
// The identifier is written as it is, with no escaping of its own.
NODE_ID_STRING_CANONICAL("nsu=urn:x;s=a;b=c", "urn:x", 's', "a;b=c")

// Namespace 0 is written bare (Part 6 §5.1.12), named by URI or not.
NODE_ID_STRING_WRITTEN("i=2258", "http://opcfoundation.org/UA/", 'i', "2258")

NODE_ID_STRING_READ("nsu=http://opcfoundation.org/UA/;i=2258", "", 0, 'i',
                    "2258")
NODE_ID_STRING_READ("ns=10;i=12345", "", 10, 'i', "12345")
NODE_ID_STRING_READ("ns=65535;s=x", "", 65535, 's', "x")
// Prefixes match in any case: ABNF quoted strings are case-insensitive
// (RFC 5234 §2.3).
NODE_ID_STRING_READ("NSU=urn:x;I=5", "urn:x", 0, 'i', "5")
NODE_ID_STRING_READ("Ns=2;S=a", "", 2, 's', "a")
NODE_ID_STRING_READ("G=09087e75-8e5e-499b-954f-f2a9603db28a", "", 0, 'g',
                    "09087E75-8E5E-499B-954F-F2A9603DB28A")
NODE_ID_STRING_READ("nsu=off%3b;i=1", "off;", 0, 'i', "1")
NODE_ID_STRING_READ("i=007", "", 0, 'i', "7")
NODE_ID_STRING_READ("i=4294967295", "", 0, 'i', "4294967295")

NODE_ID_STRING_REJECTED("")
NODE_ID_STRING_REJECTED("Plant.Temperature")
NODE_ID_STRING_REJECTED("TS.1379")
NODE_ID_STRING_REJECTED("{nsu=urn:x;i=1}")
NODE_ID_STRING_REJECTED("i=")
NODE_ID_STRING_REJECTED("i=12x")
NODE_ID_STRING_REJECTED("i=-1")
NODE_ID_STRING_REJECTED("i=+1")
NODE_ID_STRING_REJECTED("i= 1")
NODE_ID_STRING_REJECTED("i=4294967296")
NODE_ID_STRING_REJECTED("ns=65536;i=1")
NODE_ID_STRING_REJECTED("ns=;i=1")
NODE_ID_STRING_REJECTED("ns=1")
NODE_ID_STRING_REJECTED("ns=1;Plant")
NODE_ID_STRING_REJECTED("nsu=urn:x")
NODE_ID_STRING_REJECTED("nsu=;i=1")
NODE_ID_STRING_REJECTED("nsu=urn%2;i=1")
NODE_ID_STRING_REJECTED("nsu=urn%zz;i=1")
NODE_ID_STRING_REJECTED("nsu=urn:x;Plant")
NODE_ID_STRING_REJECTED("g=not-a-guid")
NODE_ID_STRING_REJECTED("g=09087E75-8E5E-499B-954F-F2A9603DB28")
NODE_ID_STRING_REJECTED("g=09087E75+8E5E-499B-954F-F2A9603DB28A")
NODE_ID_STRING_REJECTED("g={09087E75-8E5E-499B-954F-F2A9603DB28A}")
NODE_ID_STRING_REJECTED("b=AB$CD")
NODE_ID_STRING_REJECTED("b=AQ")
NODE_ID_STRING_REJECTED("b=AR==")
NODE_ID_STRING_REJECTED("b=A===")
NODE_ID_STRING_REJECTED("b=AQ==AQ==")
NODE_ID_STRING_REJECTED("x=1")
NODE_ID_STRING_REJECTED("i")
