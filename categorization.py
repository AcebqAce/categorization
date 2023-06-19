import pandas as pd
import numpy as np
import requests

company = pd.read_csv('List.csv')

company['Name'] = company['Name'].fillna('')
company['Web'] = company['Web'].fillna('')
company['Snapshot'] = company['Snapshot'].fillna('')

sector = pd.read_csv('Sectors.csv')['Sector'].tolist()
usecase = pd.read_csv('Use_Cases.csv')['Use Case'].tolist()
tech = pd.read_csv('Tech_Offerings.csv')['Tech Offering'].tolist()

categorized_companies = []

for index, row in company.iterrows():
    name = str(row['Name'])
    web = str(row['Web'])
    snap = str(row['Snapshot'])
    sectors = []
    usecases = []
    tech_offers = []

    for keyword in sector:
        if keyword in name or keyword in web or keyword in snap:
            sectors.append(keyword)

    for keyword in usecase:
        if keyword in name or keyword in web or keyword in snap:
            usecases.append(keyword)

    for keyword in tech:
        if keyword in name or keyword in web or keyword in snap:
            tech_offers.append(keyword)


    try:
        response = requests.get(web)
        if response.status_code == 200:
            webpage_content = response.text

            for keyword in sector:
                if keyword in webpage_content:
                    sectors.append(keyword)

            for keyword in usecase:
                if keyword in webpage_content:
                    usecases.append(keyword)

            for keyword in tech:
                if keyword in webpage_content:
                    tech_offers.append(keyword)

    except requests.exceptions.RequestException:
        pass

    categorized_companies.append({
        'Name': name,
        'Web': web,
        'sectors' : sectors,
        'usecases': usecases,
        'tech_offers' : tech_offers
        })

categorized_companies_df = pd.DataFrame(categorized_companies)
categorized_companies_df.to_csv("categorized_companies.csv", index=False)
