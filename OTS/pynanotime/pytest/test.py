from pynanotime import now_in_nano, num2time, time2num

print(now_in_nano())
print(num2time(now_in_nano()))
print(time2num(num2time(now_in_nano())))