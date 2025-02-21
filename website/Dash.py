import serial
import time
import dash
from dash import dcc, html
import plotly.graph_objs as go
from dash.dependencies import Input, Output
from collections import deque
import logging

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Initialize data storage
MAX_POINTS = 50
time_data = deque(maxlen=MAX_POINTS)
adc_data = deque(maxlen=MAX_POINTS)
frequency_data = deque(maxlen=MAX_POINTS)
amplitude_data = deque(maxlen=MAX_POINTS)

# Serial setup
SERIAL_PORT = 'COM9'  # Update this to match your ESP8266 port
BAUD_RATE = 115200

# Initialize Dash app
app = dash.Dash(__name__)

# Define the layout
app.layout = html.Div([
    # Header
    html.H1('Piezo Sensor Dashboard', 
            style={'textAlign': 'center', 'color': '#2c3e50', 'marginBottom': 20}),
    
    # Main content container
    html.Div([
        # Left column - Graphs
        html.Div([
            # Frequency Graph
            dcc.Graph(
                id='frequency-graph',
                style={'marginBottom': 20}
            ),
            # Amplitude Graph
            dcc.Graph(
                id='amplitude-graph'
            ),
        ], style={'width': '70%', 'display': 'inline-block', 'verticalAlign': 'top'}),
        
        # Right column - Current Values and Status
        html.Div([
            # Current Values Box
            html.Div([
                html.H3('Current Values', style={'textAlign': 'center'}),
                html.Div(id='adc-value', className='value-box'),
                html.Div(id='frequency-value', className='value-box'),
                html.Div(id='amplitude-value', className='value-box'),
            ], style={
                'padding': '20px',
                'backgroundColor': '#f8f9fa',
                'borderRadius': '10px',
                'marginBottom': '20px'
            }),
            
            # Status Box
            html.Div([
                html.H3('Status', style={'textAlign': 'center'}),
                html.Div(id='status-display', className='status-box')
            ], style={
                'padding': '20px',
                'backgroundColor': '#f8f9fa',
                'borderRadius': '10px'
            })
        ], style={'width': '29%', 'display': 'inline-block', 'verticalAlign': 'top', 'marginLeft': '1%'})
    ]),
    
    # Hidden div for storing the current status
    html.Div(id='status-store', style={'display': 'none'}),
    
    # Update interval
    dcc.Interval(
        id='interval-component',
        interval=500,  # Update every 500ms
        n_intervals=0
    )
])

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    logger.info(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud")
except serial.SerialException as e:
    logger.error(f"Failed to connect to {SERIAL_PORT}: {e}")
    ser = None

def parse_serial_data():
    """Simulate parsing static data for testing"""
    return 516, 38.0, 0.19, "Normal"  # Example static values

@app.callback(
    [Output('frequency-graph', 'figure'),
     Output('amplitude-graph', 'figure'),
     Output('adc-value', 'children'),
     Output('frequency-value', 'children'),
     Output('amplitude-value', 'children'),
     Output('status-display', 'children'),
     Output('status-display', 'style')],
    [Input('interval-component', 'n_intervals')]
)
def update_dashboard(n):
    try:
        adc, frequency, amplitude, status = parse_serial_data()
        
        current_time = time.time()
        
        # Always update with last known values
        time_data.append(current_time)
        adc_data.append(adc if adc is not None else (last_adc or 0))
        frequency_data.append(frequency if frequency is not None else (last_frequency or 0))
        amplitude_data.append(amplitude if amplitude is not None else (last_amplitude or 0))
        
        # Create frequency graph
        freq_fig = {
            'data': [{
                'x': list(time_data),
                'y': list(frequency_data),
                'type': 'scatter',
                'name': 'Frequency',
                'line': {'color': '#2ecc71'}
            }],
            'layout': {
                'title': 'Frequency Over Time',
                'xaxis': {'title': 'Time'},
                'yaxis': {'title': 'Frequency (Hz)'},
                'margin': {'l': 40, 'r': 40, 't': 40, 'b': 40}
            }
        }
        
        # Create amplitude graph
        amp_fig = {
            'data': [{
                'x': list(time_data),
                'y': list(amplitude_data),
                'type': 'scatter',
                'name': 'Amplitude',
                'line': {'color': '#3498db'}
            }],
            'layout': {
                'title': 'Amplitude Over Time',
                'xaxis': {'title': 'Time'},
                'yaxis': {'title': 'Amplitude (V)'},
                'margin': {'l': 40, 'r': 40, 't': 40, 'b': 40}
            }
        }
        
        # Status style based on current status
        status_style = {
            'backgroundColor': '#2ecc71' if status == "Normal" else '#e74c3c',
            'color': 'white',
            'padding': '20px',
            'borderRadius': '5px',
            'textAlign': 'center',
            'fontSize': '24px',
            'fontWeight': 'bold'
        }
        
        # Display values
        adc_display = f"ADC Value: {adc:.0f}" if adc is not None else "ADC Value: --"
        freq_display = f"Frequency: {frequency:.1f} Hz" if frequency is not None else "Frequency: -- Hz"
        amp_display = f"Amplitude: {amplitude:.2f} V" if amplitude is not None else "Amplitude: -- V"
        
        return [
            freq_fig,
            amp_fig,
            adc_display,
            freq_display,
            amp_display,
            status,
            status_style
        ]
    except Exception as e:
        logger.error(f"Dashboard update error: {e}")
        return [
            {'data': [], 'layout': {'title': 'Frequency Over Time'}},
            {'data': [], 'layout': {'title': 'Amplitude Over Time'}},
            "ADC Value: --",
            "Frequency: -- Hz",
            "Amplitude: -- V",
            "Error",
            {'backgroundColor': '#e74c3c', 'color': 'white', 'padding': '20px'}
        ]

if __name__ == '__main__':
    app.run_server(debug=True, use_reloader=False)
