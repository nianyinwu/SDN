# SDN
 
模擬SDN(Software-defined networking)中的switch與controller，設計Network update演算法，controller藉由此演算法計算update前後各switch的傳輸link(routing table)，並將封包依照此路徑送出。

Input : switch(node)數量、Destination數量、Link數量、初始化(配置)時間、更新時間、模擬時間、DstID、帶有權重的節點圖與資料開始傳輸的時間(data packet)、SrcID與DstID

Output : 時間、接收端ID、封包ID、來源ID、目的ID、前一個傳送的ID、下一個要傳送的ID與封包種類
