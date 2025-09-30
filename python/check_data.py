# %%
import matplotlib.pyplot as plt
import numpy as np
import mne
from pathlib import Path
from readEdf import read_edf_file, plot_signals

path = Path('C:\\Users\\liste\\Desktop\\王保增项目-运动想象\\signal-brain-32\\signbrain32\\workspace\\public\\250928093132_USB')
path = path.joinpath('EEG-restEC.edf')

path.is_file()

# %%
signals, channel_labels = read_edf_file(path.as_posix())

plot_signals(signals, channel_labels, channels_to_plot=[32])

# %%
signals[-1].shape
np.unique(signals[-1])

# %%
encoding = 'utf-8'
encoding = 'latin1'
raw = mne.io.read_raw(path, encoding=encoding)

# %%
events, event_id = mne.events_from_annotations(raw)
print(events)
print(event_id)

# %%
data = raw.get_data()
print(data.shape)
evt_data = data[-1]
print(np.unique(evt_data))

# %%
plt.figure(figsize=(12, 12))
for i in range(data.shape[0]):
    plt.plot(data[i][::2000], label=f'{i}')
plt.legend()


# %%
