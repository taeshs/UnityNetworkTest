using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Net.Sockets;
using System.Text;

using UnityEngine;
using System.Threading.Tasks;

public class NetworkSender : MonoBehaviour
{
    public string serverIp = "127.0.0.1";
    public int serverPort = 8888;
    TcpClient tcpClient;
    NetworkStream stream;
    public Transform characterTransform;
    private byte[] buffer = new byte[1024];
    Vector3 otherPos;

    public GameObject obj;
    public GameObject theobj;

    // Start is called before the first frame update
    void Start()
    {
        tcpClient = new TcpClient();
        tcpClient.Connect(serverIp, serverPort);
        stream = tcpClient.GetStream();

        theobj = Instantiate(obj, new Vector3(1, 2.9f, 1), Quaternion.identity);

        _ = SendPositionAsync();
        _ = ReceiveDataAsync();
    }

    // Update is called once per frame
    void Update()
    {
        theobj.transform.position = otherPos;
    }

    private async Task SendPositionAsync()
    {
        while (true)
        {
            try
            {
                Vector3 position = characterTransform.position;
                string positionData = $"{position.x},{position.z}";
                byte[] data = Encoding.UTF8.GetBytes(positionData);
                await stream.WriteAsync(data, 0, data.Length);
                await Task.Delay(10); // 100ms마다 위치 전송
            }
            catch (Exception ex)
            {
                Debug.LogError($"SendPositionAsync error: {ex.Message}");
                break;
            }
        }
    }

    private async Task ReceiveDataAsync()
    {
        while (true)
        {
            try
            {
                int bytesRead = await stream.ReadAsync(buffer, 0, buffer.Length);
                if (bytesRead > 0)
                {
                    string receivedData = Encoding.UTF8.GetString(buffer, 0, bytesRead);
                    ProcessReceivedData(receivedData);
                }
            }
            catch (Exception ex)
            {
                Debug.LogError($"ReceiveDataAsync error: {ex.Message}");
                break;
            }
        }
    }

    private void ProcessReceivedData(string data)
    {
        // 예시: 수신된 데이터를 처리하여 다른 클라이언트의 캐릭터 위치를 업데이트
        // 예를 들어, ParseReceivedData(data); 처리를 수행
        string[] posdata = data.Split(',');
        otherPos = new Vector3(float.Parse(posdata[0]), 2.9f, float.Parse(posdata[1]));
    }

    private void OnApplicationQuit()
    {
        stream.Close();
        tcpClient.Close();
    }
}
