import pyedflib
import numpy as np
import matplotlib.pyplot as plt

# 设置中文字体
plt.rcParams['font.sans-serif'] = ['SimHei',
                                   'Microsoft YaHei', 'DejaVu Sans']  # 用来正常显示中文标签
plt.rcParams['axes.unicode_minus'] = False  # 用来正常显示负号


def read_edf_file(file_path):
    """
    读取EDF文件并显示基本信息
    """
    try:
        # 打开EDF文件
        file = pyedflib.EdfReader(file_path)

        # 获取文件基本信息
        print("EDF文件基本信息:")
        print(f"文件路径: {file_path}")
        print(f"信号通道数: {file.signals_in_file}")
        print(f"数据点数: {file.getNSamples()[0]}")
        print(f"采样频率: {file.getSampleFrequencies()[0]} Hz")
        print(f"文件持续时间: {file.file_duration} 秒")

        fs = file.getSampleFrequencies()[0]
        info = {
            'fs': fs,
            'ch_names': ['fpz', 'c3', 'cz', ...],
            'times': [e / fs for e in range(file.getNSamples()[0])],
            'band': {
                'l_freq': 0,
                'h_freq': 500,
            }
        }
        event_id = {}
        # points, 0 , event_int
        events = np.zeros((1000, 3))

        # 获取通道标签
        channel_labels = file.getSignalLabels()
        print(f"\n通道标签: {channel_labels}")

        # 读取所有通道的数据
        signals = []
        for i in range(file.signals_in_file):
            signal = file.readSignal(i)
            signals.append(signal)
            print(f"通道 {i} ({channel_labels[i]}): {len(signal)} 个数据点")

        # 关闭文件
        file.close()

        return signals[:-1], channel_labels -> info['ch_names'], info, events, event_id

    except Exception as e:
        print(f"读取EDF文件时出错: {e}")
        return None, None


def plot_signals(signals, channel_labels, start=0, end=None, channels_to_plot=None):
    """
    绘制信号图
    """
    if end is None:
        end = len(signals[0])

    if channels_to_plot is None:
        channels_to_plot = range(len(signals))

    fig, axes = plt.subplots(len(channels_to_plot), 1, figsize=(12, 8))
    if len(channels_to_plot) == 1:
        axes = [axes]

    for i, channel_idx in enumerate(channels_to_plot):
        signal = signals[channel_idx][start:end]
        axes[i].plot(signal)
        axes[i].set_title(f'通道 {channel_idx}: {channel_labels[channel_idx]}')
        axes[i].set_ylabel('幅度')

    axes[-1].set_xlabel('采样点')
    plt.tight_layout()
    plt.show()


# 使用示例
if __name__ == "__main__":
    # 替换为您的EDF文件路径
    edf_file_path = "EEG-restEC.edf"

    # 读取EDF文件
    signals, channel_labels = read_edf_file(edf_file_path)

    # 通道值打印
    # for i in signals[32]:
    #     print(i)

    if signals is not None:
        plot_signals(signals, channel_labels, end=58560, channels_to_plot=[32])
