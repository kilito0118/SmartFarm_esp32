let ws;
let charts = {};

// 데이터 버퍼 (시간 순서 그래프용)
let dataBuffer = {
  temperature: [],
  humidity: [],
  ldr: [],
  soil: [],
  labels: [],
};
function createLineChart(ctx, label, color, yMin, yMax) {
  return new Chart(ctx, {
    type: 'line',
    data: {
      labels: [],
      datasets: [{
        label: label,
        data: [],
        borderColor: color,
        borderWidth: 2,
        fill: false,
        tension: 0.3
      }]
    },
    options: {
      responsive: true,
      animation: false,
      scales: {
        y: {
          beginAtZero: true,
          min: yMin,
          max: yMax
        }
      }
    }
  });
}

function updateChart(chart, value, label = "") {
  const maxPoints = 20;
  if (chart.data.labels.length >= maxPoints) {
    chart.data.labels.shift();
    chart.data.datasets[0].data.shift();
  }
  chart.data.labels.push(label || new Date().toLocaleTimeString());
  chart.data.datasets[0].data.push(value);
  chart.update();
}
function initCharts() {
  charts.temperature = createLineChart(
    document.getElementById('tempChart'),
    '온도 (℃)',
    'rgba(231, 76, 60, 1)',
    0, 50  // 온도 범위 (0~50℃)
  );
  charts.humidity = createLineChart(
    document.getElementById('humidityChart'),
    '습도 (%)',
    'rgba(52, 152, 219, 1)',
    0, 100 // 습도 범위 (0~100%)
  );
  charts.ldr = createLineChart(
    document.getElementById('ldrChart'),
    '조도',
    'rgba(241, 196, 15, 1)',
    0, 1200 // 조도 예시 범위
  );
  charts.soil = createLineChart(
    document.getElementById('soilChart'),
    '토양 습도',
    'rgba(46, 204, 113, 1)',
    0, 4095 // 토양습도 센서의 최대값
  );
}
function connectWebSocket() {
  const wsProtocol = window.location.protocol === 'https:' ? 'wss' : 'ws';
  const wsHost = window.location.host;
  const wsUrl = `${wsProtocol}://${wsHost}/ws/client`;

  ws = new WebSocket(wsUrl);

  ws.onopen = () => {
    document.querySelector("#ws-status span").textContent = "Connected";
    document.querySelector("#ws-status span").className = "connected";
  };
ws.onmessage = (event) => {
  try {
    const raw = JSON.parse(event.data);

    // 키 이름 통일(수신데이터에 따라 보정)
    const temp = parseFloat(raw.temp ?? raw.temperature);
    const hum = parseFloat(raw.hum ?? raw.humidity);
    const ldr = parseInt(raw.ldr);
    const soil = parseInt(raw.soil);

    if (!isNaN(temp) && temp >= 0 && temp <= 50)
      updateChart(charts.temperature, temp);
    if (!isNaN(hum) && hum >= 0 && hum <= 100)
      updateChart(charts.humidity, hum);
    if (!isNaN(ldr) && ldr >= 0)
      updateChart(charts.ldr, ldr);
    if (!isNaN(soil) && soil >= 0)
      updateChart(charts.soil, soil);
  } catch (err) {
    console.error("데이터 파싱 오류:", err);
  }
};

  ws.onclose = () => {
    document.querySelector("#ws-status span").textContent = "Disconnected";
    document.querySelector("#ws-status span").className = "disconnected";
    setTimeout(connectWebSocket, 3000); // 재연결 시도
  };
}

window.onload = function () {
  initCharts();
  connectWebSocket();
};
