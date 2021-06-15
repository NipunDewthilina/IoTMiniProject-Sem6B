var i;
var j;
let safety_factor = 0.001;
let total = 0;
let districts = ["Ampara",
        "Anuradhapura",
        "Badulla",
        "Batticaloa",
        "Colombo",
        "Galle",
        "Gampaha",
        "Hambantota",
        "Jaffna",
        "Kalutara",
        "Kandy",
        "Kegalle",
        "Kilinochchi",
        "Kurunegala",
        "Mannar",
        "Matale",
        "Matara",
        "Moneragala",
        "Mullaitivu",
        "Nuwara Eliya",
        "Polonnaruwa",
        "Puttalam",
        "Ratnapura",
        "Trincomalee",
        "Vavuniya"]
        
let district_pop = [648057,
856232,
811758,
525142,
2309809,
1058771,
2294641,
596617,
583378,
1217260,
1369899,
836603,
112875,
1610299,
99051,
482229,
809344,
448142,
91947,
706588,
403335,
759776,
1082277,
378182,
171511,
2026372]
let data = global.get("data");
let result_safety_fac = {};
let result = {};
for (i = 0; i < 25; i++) {
     total =0
    // do something with msg.payload[i]
    for (j = 0;j < msg.districts_data[districts[i]].number_of_hos;j++){
        total += data[msg.districts_data[districts[i]].Hospital_name[j]-1].cumulative_local;
    }
    result_safety_fac[districts[i]] = total/district_pop[i];
    result[districts[i]] = total;
}

return{
    total:total,
    data:data,
    districts_data:msg.districts_data,
    districts:districts,
    result:result,
    safety_facs:result_safety_fac
};

