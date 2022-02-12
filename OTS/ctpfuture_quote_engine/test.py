from pprint import pprint
from ctpfuture_quote_engine import run_quote_engine

front_address = "tcp://180.168.146.187:10211"
universe = ["IF2202", "IF2203", "i2205"]
level = "trace"

run_quote_engine(front_address=front_address, universe=universe, level=level)