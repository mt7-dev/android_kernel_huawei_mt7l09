Kernel for Huawei Ascend Mate 7
===============================

This is the stock kernel 3.10 downloaded from huawei site http://emui.huawei.com/en/plugin.php?id=hwdownload&mod=detail&mid=74 with checksum md5 *0F3AAFD9B7B136993DF3C29D9D31C43A*.

Changes
-------
- /drivers/modem was a broken symlink to *../../vendor/hisi/modem/drv/acore/kernel/drivers/modem*. So I took the folder vendor from h60 kernel https://github.com/codeworkx/kernel-huawei-h60 and I put it in drivers/. Now drivers/modem is symlinked to */drivers/vendor/hisi/modem/drv/acore/kernel/drivers/modem*.

Notes
-----
- The file config-stock511 is the config file extracted from stock lollipop ROM (B324) with extract-ikconfig in scripts/ folder;
- I don't understand why there is a folder jazz.tar.gz with the same kernel folder.
