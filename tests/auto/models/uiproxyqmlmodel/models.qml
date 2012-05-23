import QtQuick 2.0

Item {
    property variant arrayList: ["Brazil", "Monaco", 1234, true]
    property variant quickList: ListModel {
        ListElement { name: "John"; age: 20 }
        ListElement { name: "Paul"; age: 32 }
        ListElement { name: "Peter"; age: 14 }
    }
    property variant invalidList: "Hello world!"
}
