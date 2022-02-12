from pprint import pprint
import ctpfuture_client

static_data = ctpfuture_client.get_static_data()
pprint(static_data)