import requests
import json

response = requests.get("https://www.hpb.health.gov.lk/api/get-current-statistical")

json_data = response.json()
array = (json_data['data']['hospital_data'])
result = {}
i = 0
hospitals = []
for each in array:
    print(each['hospital']['id'],each['hospital']['name'],each['hospital']['name_si'])
    hospitals.append(each['hospital']['name'])


districts = ['Ampara',
'Anuradhapura',
'Badulla',
'Batticaloa',
'Colombo',
'Galle',
'Gampaha',
'Hambantota',
'Jaffna',
'Kalutara',
'Kandy',
'Kegalle',
'Kilinochchi',
'Kurunegala',
'Mannar',
'Matale',
'Matara',
'Moneragala',
'Mullaitivu',
'Nuwara Eliya',
'Polonnaruwa',
'Puttalam',
'Ratnapura',
'Trincomalee',
'Vavuniya']

districts_dict = {'Ampara':{"Hospital_name":[], 'number_of_hos':0},
'Anuradhapura':{"Hospital_name":[5], 'number_of_hos':1},
'Badulla':{"Hospital_name":[13], 'number_of_hos':1},
'Batticaloa':{"Hospital_name":[9,
                               34],
              'number_of_hos':2},
'Colombo':{"Hospital_name":[14,15,17,
                            18,
                            24,
                            25,
                            26,
                            29,
                            32,
                            23,
                            36,
                            1,
                            2],
           'number_of_hos':13},
'Galle':{"Hospital_name":[4], 'number_of_hos':1},
'Gampaha':{"Hospital_name":[10,
                            11,
                            3,
                            35],
           'number_of_hos':4},
'Hambantota':{"Hospital_name":[19],
              'number_of_hos':1},
'Jaffna':{"Hospital_name":[7], 'number_of_hos':1},
'Kalutara':{"Hospital_name":[22,
                             33],
            'number_of_hos':2},
'Kandy':{"Hospital_name":[8],
         'number_of_hos':1},
'Kegalle':{"Hospital_name":[], 'number_of_hos':0},
'Kilinochchi':{"Hospital_name":[], 'number_of_hos':0},
'Kurunegala':{"Hospital_name":[6], 'number_of_hos':1},
'Mannar':{"Hospital_name":[], 'number_of_hos':0},
'Matale':{"Hospital_name":[], 'number_of_hos':0},
'Matara':{"Hospital_name":[28], 'number_of_hos':1},
'Moneragala':{"Hospital_name":[20],
              'number_of_hos':1},
'Mullaitivu':{"Hospital_name":[], 'number_of_hos':0},
'Nuwara Eliya':{"Hospital_name":[], 'number_of_hos':0},
'Polonnaruwa':{"Hospital_name":[16,
                                21],
               'number_of_hos':2},
'Puttalam':{"Hospital_name":[27,
                             31],
            'number_of_hos':2},
'Ratnapura':{"Hospital_name":[12], 'number_of_hos':1},
'Trincomalee':{"Hospital_name":[], 'number_of_hos':0},
'Vavuniya':{"Hospital_name":[30], 'number_of_hos':1}}

with open('app.json', 'w') as fp:
    json.dump(districts_dict, fp)

app_json = json.dumps(districts_dict)
    
