import { renderHook, act } from '@testing-library/react'
import { describe, it, expect } from 'vitest'
import { MaintenanceProvider } from '../../../frontend/src/context/MaintenanceContext'
import { useMachineStats, useLiveSearch } from '../../../frontend/src/context/useHooks'

describe('useMachineStats', () => {

  it('Boş makine listesinde değerler 0', () => {
    const wrapper = ({ children }) => (
      <MaintenanceProvider>{children}</MaintenanceProvider>
    )

    const { result } = renderHook(() => useMachineStats(), { wrapper })

    expect(result.current.total).toBe(0)
    expect(result.current.healthScore).toBe(0)
  })
})

describe('useLiveSearch', () => {

  it('Filtreleme çalışıyor', () => {
    const items = [
      { name: 'Pump' },
      { name: 'Motor' }
    ]

    const { result } = renderHook(() =>
      useLiveSearch(items, 'name')
    )

    act(() => {
      result.current.setQuery('pump')
    })

    expect(result.current.filteredItems.length).toBe(1)
  })
})