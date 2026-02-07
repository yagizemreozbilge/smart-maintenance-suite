import React, { createContext, useContext, useReducer } from 'react';

// Point 5: Reducer - Central decisions center
// Point 2: Structure - Status based instead of multiple booleans
const maintenanceReducer = (state, action) => {
    switch (action.type) {
        case 'UPDATE_STOCK':
            return { ...state, stock: action.payload };
        case 'ADD_MACHINE':
            return { ...state, machines: [...state.machines, action.payload] };
        case 'SET_STATUS':
            return { ...state, appStatus: action.payload };
        case 'REMOVE_MACHINE':
            return { ...state, machines: state.machines.filter(m => m.id !== action.payload) };
        default:
            return state;
    }
};

const initialState = {
    stock: 3,
    machines: [
        { id: 1, name: "CNC Lathe-01", status: "Operational" },
        { id: 2, name: "Robot Arm-04", status: "Warning" },
    ],
    alerts: [
        { id: 3, sensor_id: 1, severity: "CRITICAL", message: "Critical Temperature detected: 95.50 C", created_at: "2026-01-29 22:51:38" },
        { id: 2, sensor_id: 1, severity: "CRITICAL", message: "Critical Temperature detected: 95.50 C", created_at: "2026-01-29 22:44:41" },
        { id: 1, sensor_id: 1, severity: "Medium", message: "Temperature is approaching the critical threshold (85C)!", created_at: "2026-01-20 23:08:38" }
    ],
    appStatus: 'idle'
};

// Point 6: Context - Data teleportation
const MaintenanceContext = createContext(null);
const MaintenanceDispatchContext = createContext(null);

// Point 7: Scaling with Reducer + Context
export function MaintenanceProvider({ children }) {
    const [state, dispatch] = useReducer(maintenanceReducer, initialState);

    return (
        <MaintenanceContext.Provider value={state}>
            <MaintenanceDispatchContext.Provider value={dispatch}>
                {children}
            </MaintenanceDispatchContext.Provider>
        </MaintenanceContext.Provider>
    );
}

// Custom hooks for easier access
export function useMaintenance() {
    return useContext(MaintenanceContext);
}

export function useMaintenanceDispatch() {
    return useContext(MaintenanceDispatchContext);
}
