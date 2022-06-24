from pickle import dumps, loads
from pyshmkv import PyShmKV as _PyShmKV


class PyShmKV(_PyShmKV):
    def __init__(self,
                 path: str,
                 count: int,
                 value_size: int,
                 write_mode: bool,
                 init_disk: bool,
                 thread_lock: bool,
                 process_lock: bool,
                 process_mutex: str = "process_mutex"):
        """

        :param path:
        :param count:
        :param value_size:
        :param write_mode:
        :param init_disk:
        :param thread_lock:
        :param process_lock:
        :param process_mutex:
        """
        super().__init__(path, count, value_size, write_mode, init_disk, thread_lock, process_lock, process_mutex)

    def set_data(self, key: str, value:any):
        """
        任意数据通过pickle转换为二进制数据
        :param key:
        :param value:
        :return:
        """
        bin_data = dumps(value, 0)
        return self.set(key, bin_data, len(bin_data)+1)

    def set_data_thread_mutex(self, key: str, value):
        """
        任意数据通过pickle转换为二进制数据
        :param key:
        :param value:
        :return:
        """
        bin_data = dumps(value, 0)
        return self.set_thread_mutex(key, bin_data, len(bin_data))

    def set_data_process_mutex(self, key: str, value):
        """
        任意数据通过pickle转换为二进制数据
        :param key:
        :param value:
        :return:
        """
        bin_data = dumps(value, 0)
        return self.set_process_mutex(key, bin_data, len(bin_data))

    def get_data(self, key: str):
        ret, bin_data = self.get(key)
        if ret:
            return ret, None
        else:
            return ret, loads(bin_data.encode())

    def del_data(self, key):
        self.del_void(key)


if __name__ == "__main__":
    from pandas import DataFrame

    shm = PyShmKV("test.store", 128, 1024, True, True, False, False)
    # value = str(1234567890123456789012345678901234567890)
    # print(shm.set("test", value, len(value)))
    # print(shm.get("test"))

    data = DataFrame(data=[1, 2, 3], columns=["test"])
    bin_data = dumps(data, 0)
    print(type(bin_data))
    # bin_data = bin_data.decode("latin1").encode("utf-8").decode("utf-8")

    print(bin_data)
    shm.set("test", bin_data, len(bin_data))
    ret, value = shm.get("test")
    print(loads(value.encode()))

    shm.set_data("test1", data)
    print(shm.get_data("test1"))

    from pandas import DataFrame
    data = DataFrame(data=[1, 2, 3], columns=["test"])
    bin_data = dumps(data, 0)
    print(type(bin_data))

    shm.set_data("test1", data)
    print(shm.get_data("test1"))






