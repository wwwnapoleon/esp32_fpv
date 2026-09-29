"""
MJPEG Relay Server
Забирает поток с ESP32 и отдает его на внешний сайт.
"""
import asyncio
import aiohttp
from aiohttp import web
import argparse
import logging

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("relay")

# ====== НАСТРОЙКИ ======
ESP32_IP = "192.168.100.50"   # <-- ЗАМЕНИТЕ на IP вашего ESP32 (из Serial Monitor)
ESP32_STREAM_PORT = 80
RELAY_PORT = 8080

# Разрешаем CORS, чтобы GitHub Pages мог обращаться
CORS_HEADERS = {
    "Access-Control-Allow-Origin": "*",
    "Access-Control-Allow-Methods": "GET, OPTIONS",
    "Access-Control-Allow-Headers": "Content-Type",
}


async def stream_proxy(request):
    """
    Проксирует MJPEG-поток с ESP32.
    """
    url = f"http://{ESP32_IP}:{ESP32_STREAM_PORT}/stream"
    logger.info(f"Client connected, proxying {url}")

    response = web.StreamResponse(
        status=200,
        headers={
            "Content-Type": "multipart/x-mixed-replace; boundary=frame",
            **CORS_HEADERS,
        },
    )
    await response.prepare(request)

    try:
        async with aiohttp.ClientSession() as session:
            async with session.get(url) as esp_response:
                if esp_response.status != 200:
                    logger.error(f"ESP32 returned {esp_response.status}")
                    return response

                async for chunk in esp_response.content.iter_any():
                    try:
                        await response.write(chunk)
                    except (ConnectionResetError, asyncio.CancelledError):
                        logger.info("Client disconnected")
                        break
    except Exception as e:
        logger.error(f"Stream error: {e}")
    finally:
        logger.info("Stream closed")

    return response


async def status_proxy(request):
    """Проксирует /status с ESP32."""
    url = f"http://{ESP32_IP}:{ESP32_STREAM_PORT}/status"
    try:
        async with aiohttp.ClientSession() as session:
            async with session.get(url) as resp:
                data = await resp.text()
                return web.Response(text=data, content_type="application/json",
                                    headers=CORS_HEADERS)
    except Exception as e:
        return web.Response(text=f'{{"error":"{e}"}}', status=502,
                            content_type="application/json", headers=CORS_HEADERS)


async def servo_proxy(request):
    """Проксирует /servo?pan=X&tilt=Y."""
    pan = request.query.get("pan", "90")
    tilt = request.query.get("tilt", "90")
    url = f"http://{ESP32_IP}:{ESP32_STREAM_PORT}/servo?pan={pan}&tilt={tilt}"
    try:
        async with aiohttp.ClientSession() as session:
            async with session.get(url) as resp:
                data = await resp.text()
                return web.Response(text=data, content_type="application/json",
                                    headers=CORS_HEADERS)
    except Exception as e:
        return web.Response(text=f'{{"error":"{e}"}}', status=502,
                            content_type="application/json", headers=CORS_HEADERS)


async def health(request):
    return web.Response(text="OK", headers=CORS_HEADERS)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--esp-ip", default=ESP32_IP, help="IP ESP32")
    parser.add_argument("--port", type=int, default=RELAY_PORT, help="Порт релея")
    args = parser.parse_args()

    global ESP32_IP
    ESP32_IP = args.esp_ip

    app = web.Application()
    app.router.add_get("/stream", stream_proxy)
    app.router.add_get("/status", status_proxy)
    app.router.add_get("/servo", servo_proxy)
    app.router.add_get("/health", health)
    app.router.add_route("OPTIONS", "/{tail:.*}", lambda r: web.Response(headers=CORS_HEADERS))

    logger.info(f"Relay started on http://0.0.0.0:{args.port}")
    logger.info(f"Proxying ESP32 at http://{ESP32_IP}:{ESP32_STREAM_PORT}")
    web.run_app(app, host="0.0.0.0", port=args.port)


if __name__ == "__main__":
    main()
