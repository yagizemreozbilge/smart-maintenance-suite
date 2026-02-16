import { render, screen } from '@testing-library/react'
import { describe, it, expect } from 'vitest'
import StatusCard from '../../frontend/src/components/StatusCard'

describe('StatusCard', () => {

  it('Prop doğru render edilir', () => {
    render(<StatusCard label="Total" value={5} status="ok" />)

    expect(screen.getByText('Total')).toBeInTheDocument()
    expect(screen.getByText('5')).toBeInTheDocument()
  })

  it('Status branch çalışır', () => {
    render(<StatusCard label="Critical" value={1} status="critical" />)
    expect(screen.getByText('Critical')).toBeInTheDocument()
  })
})