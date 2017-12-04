from datetime import datetime
import csv
from matplotlib import pyplot as plt

filename = 'daily-engagement.csv'
with open(filename) as f:
    reader = csv.reader(f)
    head_low = next(reader)

    dates, total_minutes, acct = [],[],[]
    for row in reader:
        minutes = int(row[3])
        total_minutes.append(minutes)
        date = datetime.strptime(row[1], "%Y/%m/%d")
        dates.append(date)
        acct_a = int(row[0])
        acct.append(acct_a)
        if(acct_a != 0):
            break

    fig = plt.figure(dpi = 128,figsize = (10,6))
    plt.plot(dates,total_minutes,c='red')

    plt.title("daily-engegament",fontsize = 24)
    plt.xlabel('',fontsize = 16)
    fig.autofmt_xdate()
    plt.ylabel("minutes",fontsize = 16)
    plt.tick_params(axis = 'both', which = 'major',labelsize=16)

    plt.show()